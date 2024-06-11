import React, {SyntheticEvent, useCallback, useEffect, useRef, useState} from 'react'
import Typography from '@mui/material/Typography'
import Container from '@mui/material/Container'
import {PB} from 'splitflapjs-proto'
import {Alert, AlertTitle, AppBar, Button, Card, CardContent, Checkbox, CircularProgress, Dialog, DialogActions, DialogContent, DialogContentText, DialogTitle, FormControlLabel, Link, Toolbar, Tooltip,} from '@mui/material'
import {NoUndefinedField} from './util'
import {SplitflapWebSerial} from 'splitflapjs-webserial'
import { applyResetModule, applySetFlaps } from 'splitflapjs-core/dist/util'

const FLAPS = [
    ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
    'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
    'Z', 'g', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'r',
    '.', '?', '-', '$', '\'', '#', 'y', 'p', ',', '!', '@', '&', 'w',
]
const legalString = (s: string) => {
    for (const c of s) {
        if (!FLAPS.includes(c)) {
            return false
        }
    }
    return true
}

type Config = NoUndefinedField<PB.ISplitflapConfig>

const defaultConfig: Config = {
    modules: []
}

type LogLine = [Date, string]
type LogDisplay = {
    lastN: number,
    after?: Date,
    title: string,
    body: string,
}

export type AppProps = object
export const App: React.FC<AppProps> = () => {
    const [splitflap, setSplitflap] = useState<SplitflapWebSerial | null>(null)
    const [splitflapState, setSplitflapState] = useState<NoUndefinedField<PB.ISplitflapState>>(
        PB.SplitflapState.toObject(PB.SplitflapState.create(), {
            defaults: true,
        }) as NoUndefinedField<PB.ISplitflapState>,
    )
    const [splitflapGeneralState, setSplitflapGeneralState] = useState<NoUndefinedField<PB.IGeneralState> | null>();
    const [inputValue, setInputValue] = useState({val: '', user: true});

    const handleInputChange = (event: React.ChangeEvent<HTMLInputElement>) => {
      const { value } = event.target;
      const upper = value.toUpperCase()
      if (legalString(upper)) {
        setInputValue({val: upper, user: true});
      }
    };
    const [splitflapConfig, setSplitflapConfig] = useState<Config>(defaultConfig)
    useEffect(() => {
        console.log('send config', splitflapConfig)
        splitflap?.sendConfig(PB.SplitflapConfig.create(splitflapConfig))
    }, [
        splitflap,
        splitflapConfig.modules,
    ])

    useEffect(() => {
        if (splitflapConfig.modules.length !== splitflapState.modules.length) {
            setSplitflapConfig({
                modules: Array(splitflapState.modules.length).fill(null).map(() => {
                    return {targetFlapIndex: 0, resetNonce: 0, movementNonce: 0}
                })
            })
        }
    }, [splitflapConfig, splitflapState])

    const [splitflapLogs, setSplitflapLogs] = useState<Array<LogLine>>([])
    const [unsavedCalibration, setUnsavedCalibration] = useState<boolean>(false)
    const [logDisplay, setLogDisplay] = useState<LogDisplay | null>(null);

    const initializationTimeoutRef = useRef<ReturnType<typeof setTimeout>>()

    const connectToSerial = async () => {
        try {
            if (navigator.serial) {
                const serialPort = await navigator.serial.requestPort({
                    filters: SplitflapWebSerial.USB_DEVICE_FILTERS,
                })
                serialPort.addEventListener('disconnect', () => {
                    setSplitflap(null)
                })
                const splitflap = new SplitflapWebSerial(serialPort, (message) => {
                    if (message.payload === 'splitflapState' && message.splitflapState !== null) {
                        const state = PB.SplitflapState.create(message.splitflapState)
                        const stateObj = PB.SplitflapState.toObject(state, {
                            defaults: true,
                        }) as NoUndefinedField<PB.ISplitflapState>
                        setSplitflapState(stateObj)
                    } else if (message.payload === 'log' && message.log !== null) {
                        const newLog = message.log?.msg
                        console.log('LOG from splitflap', newLog)
                        if (newLog != null) {
                            const ts = new Date()
                            setSplitflapLogs((cur) => {
                                const newLogs = cur.slice(-30)
                                newLogs.push([ts, newLog])
                                return newLogs
                            })
                        }
                    } else if (message.payload === 'ack') {
                        // Ignore (internal protocol implementation detail)
                    } else if (message.payload === 'generalState' && message.generalState !== null) {
                        const state = PB.GeneralState.create(message.generalState)
                        const stateObj = PB.GeneralState.toObject(state, {
                            defaults: true,
                        }) as NoUndefinedField<PB.IGeneralState>
                        setSplitflapGeneralState(stateObj)

                        const initializationTimeout = initializationTimeoutRef.current;
                        if (initializationTimeout !== undefined) {
                            clearTimeout(initializationTimeout)
                            initializationTimeoutRef.current = undefined;
                            setSplitflap(splitflap)
                        }
                    } else {
                        console.log('Unhandled message type', message);
                    }
                })
                const loop = splitflap.openAndLoop()
                splitflap.sendConfig(PB.SplitflapConfig.create(splitflapConfig))

                // Older firmware did not send general state; use a timeout to determine if we should fall back to legacy mode
                initializationTimeoutRef.current = setTimeout(() => {
                    initializationTimeoutRef.current = undefined
                    console.log('Timed out waiting for initial general state; assuming this is a legacy splitflap connected')
                    setSplitflap(splitflap)
                }, 3000)
                await loop
            } else {
                console.error('Web Serial API is not supported in this browser.')
                setSplitflap(null)
            }
        } catch (error) {
            console.error('Error with serial port:', error)
            setSplitflap(null)
        }
    }

    const [forceFullRotations, setForceFullRotations] = useState<boolean>(false)
    const updateSplitflap = useCallback((value: string) => {
        // TODO: should probably change types and use applySetFlaps?
        setSplitflapConfig((cur) => {
            const newModules = []
            for (let i = 0; i < splitflapState.modules.length; i++) {
                const targetFlapIndex = value[i] !== undefined ? FLAPS.indexOf(value[i]) : 0
                newModules.push({
                    targetFlapIndex,
                    resetNonce: cur.modules[i]?.resetNonce ?? 0,
                    movementNonce: (cur.modules[i]?.movementNonce ?? 0) + (forceFullRotations ? (targetFlapIndex === 0 ? 0 : 1) : 0),
                })
            }
            return {
                modules: newModules,
            }
        })
    }, [splitflapState.modules])


    const numModules = splitflapState.modules.length
    const charWidth = Math.max(1000 / numModules, 40)

    return (
        <>
        <AppBar position="relative" color="default">
          <Toolbar>
            <Typography variant="h6" color="inherit" noWrap>
              Splitflap Web Serial Demo
            </Typography>
          </Toolbar>
        </AppBar>
            <Container component="main" maxWidth="lg">
        <Card sx={{margin: '32px'}}>
            <CardContent>
                    {splitflap !== null ? (
                        <>
                            {unsavedCalibration ? (
                                <Alert
                                severity="warning"
                                action={
                                    <Button color="inherit" size="small" onClick={() => {
                                        setLogDisplay({
                                            lastN: 20,
                                            after: new Date(),
                                            title: "Saving calibration...",
                                            body: "Check the logs to confirm the calibration has saved successfully:"
                                        })
                                        setTimeout(() => splitflap.saveAllOffsets(), 200)
                                        setUnsavedCalibration(false)
                                    }}>
                                    SAVE CALIBRATION
                                    </Button>
                                }
                                >
                                <AlertTitle>Unsaved calibration</AlertTitle>
                                Module calibration has been modified but has not been saved yet. It will be lost when the ESP32 is restarted.
                                </Alert>
                            ) : null}
                            <Typography variant="h4" color="inherit">
                                Current state
                            </Typography>
                            <div style={{
                            }}>
                            {
                                splitflapState.modules.map((module, i) => {
                                    return (<SplitflapModuleDisplay
                                            i={i}
                                            module={module}
                                            charWidth={charWidth}
                                            setSplitflapConfig={setSplitflapConfig}
                                            increaseOffsetTenth={() => splitflap?.offsetIncrementTenth(i)}
                                            increaseOffsetHalf={() => splitflap?.offsetIncrementHalf(i)}
                                            goToFlap={(flapIndex) => {
                                                const update: (number | null)[] = Array(i + 1).fill(null)
                                                update[i] = flapIndex
                                                setSplitflapConfig((curConfig) => {
                                                    return PB.SplitflapConfig.toObject(applySetFlaps(PB.SplitflapConfig.create(curConfig), update), {
                                                        defaults: true,
                                                    }) as NoUndefinedField<PB.ISplitflapConfig>
                                                })
                                            }}
                                            setOffsetToCurrentStep={
                                                () => {
                                                    splitflap?.offsetSetToCurrentStep(i)
                                                    setUnsavedCalibration(true);
                                                }
                                            }
                                        />)
                                })
                            }
                            </div>
                            <br />
                            <Typography variant="h4" color="inherit">
                                Input
                            </Typography>
                            <form onSubmit={(event) => {
                                    event.preventDefault()
                                    updateSplitflap(inputValue.val)
                                    setInputValue({val: '', user: true})
                                }}>
                                    <div style={{
                                        width: `${charWidth * numModules}px`,
                                        overflow: 'hidden',
                                    }}>
                                        <div style={{
                                            left: 0,
                                            position: 'sticky'
                                        }}>
                                            <input 
                                                type="text"
                                                maxLength={numModules}
                                                onChange={handleInputChange}
                                                value={inputValue.val}
                                                onBlur={e => e.target.focus()}
                                                spellCheck="false"
                                                style={{
                                                    color: '#333',
                                                    caret: 'block',
                                                    paddingLeft: `${charWidth*0.12}px`,
                                                    paddingTop: '20px',
                                                    paddingBottom: '20px',
                                                    letterSpacing: `${0.4 * charWidth}px`,
                                                    border: 0,
                                                    outline: 'none',
                                                    backgroundImage: 'url("outline.svg")',
                                                    backgroundSize: `${charWidth}px`,
                                                    backgroundRepeat: 'repeat-x',
                                                    backgroundPosition: `0px ${20 + charWidth*0.1}px`,
                                                    fontSize: `${charWidth}px`,
                                                    fontFamily: 'Roboto Mono',
                                                    width: `${charWidth * numModules + 50}px`,
                                                }}
                                                />
                                        </div>
                                    </div>
                            </form>
                            <FormControlLabel control={<Checkbox checked={forceFullRotations} onChange={() => setForceFullRotations((cur) => !cur)} />} label="Force full rotations" />
                            <br />
                            <Link onClick={() => {
                                setLogDisplay({lastN: 20, title: "Recent logs", body:""})
                            }}>View logs</Link>
                            <pre>{ JSON.stringify(splitflapGeneralState) }</pre>
                            { logDisplay !== null ? (
                                <Dialog open={true} onClose={() => setLogDisplay(null)}>
                                    <DialogTitle>{logDisplay.title}</DialogTitle>
                                    <DialogContent>
                                    <DialogContentText>
                                        {logDisplay.body}
                                    </DialogContentText>
                                    <Logs
                                        logs={splitflapLogs} 
                                        lastN={20}
                                        after={logDisplay.after}
                                        />
                                </DialogContent>
                                <DialogActions>
                                    <Button variant='contained' onClick={() => {
                                        setLogDisplay(null)
                                    }}>Done</Button>
                                </DialogActions>
                                </Dialog>
                            ) : null }
                        </>
                    ) : navigator.serial ? (
                        <>
                        <Typography variant="h4" color="inherit">
                            Welcome
                        </Typography>
                        <Typography variant="body1">
                            <p>If you have a Splitflap Display built with the Chainlink electronics system and you have up-to-date firmware installed on it,
                            you can connect it via USB and control it using this web page. This uses Web Serial to talk to the device without needing to
                            install any software on your computer.</p>
                        </Typography>
                        <Button onClick={connectToSerial} variant="contained">
                            Connect via Web Serial
                        </Button>
                        </>
                    ) : (
                        <Typography>
                            Sorry, Web Serial API isn't available in your browser. Try the latest version of Chrome.
                        </Typography>
                    )}

                    { splitflap === null ?
                        <Typography variant="body1">
                            <p><b>Haven't built a display yet, or want to learn more?</b> Check out the <Link href="3d_viewer/">project landing page</Link> to see
                            an interactive 3d model and read more about the project.</p>
                        </Typography>
                        :
                        null
                    }
                    </CardContent>
                    </Card>
            </Container>
        </>
    )
}

type SplitflapModuleDisplayProps = {
    charWidth: number,
    i: number,
    setSplitflapConfig: React.Dispatch<React.SetStateAction<NoUndefinedField<PB.ISplitflapConfig>>>,
    module: NoUndefinedField<PB.SplitflapState.IModuleState>,
    increaseOffsetTenth: () => void,
    increaseOffsetHalf: () => void,
    goToFlap: (i: number) => void,
    setOffsetToCurrentStep: () => void,
}

enum CalibrationStep {
    FIND_FLAP_BOUNDARY = 0,
    ADJUST_WHOLE_FLAP_OFFSET = 1,
    CALIBRATING = 2,
    CONFIRM = 3,
}

const SplitflapModuleDisplay: React.FC<SplitflapModuleDisplayProps> = (props) => {
    const {charWidth, i, setSplitflapConfig, module, increaseOffsetTenth, increaseOffsetHalf, goToFlap, setOffsetToCurrentStep } = props
    const [dialogOpen, setDialogOpen] = useState<boolean>(false)
    const [calibrationStep, setCalibrationStep] = useState<CalibrationStep>(CalibrationStep.FIND_FLAP_BOUNDARY);

    const calibrationComponent: Record<CalibrationStep, React.FC<void>> = {
        [CalibrationStep.FIND_FLAP_BOUNDARY]: () => 
            <>
            <DialogContent>
                <DialogContentText>
                    Keep clicking this button until the flap flips...
                </DialogContentText>
                <Button variant='contained' onClick={() => {
                    increaseOffsetTenth()
                }}>&gt;&gt;</Button>
                <br />
                <DialogContentText>
                    then click Continue.
                </DialogContentText>
                <br />
                <DialogContentText variant='caption'>
                    (If you accidentally go too far, just keep clicking until the next flap flips)
                </DialogContentText>
            </DialogContent>
            <DialogActions>
                <Button variant='outlined' onClick={() => {
                    increaseOffsetHalf()
                    setCalibrationStep(CalibrationStep.ADJUST_WHOLE_FLAP_OFFSET)
                }}>Continue</Button>
            </DialogActions>
            </>,
        [CalibrationStep.ADJUST_WHOLE_FLAP_OFFSET]: () => <>
        <DialogContent>
            <DialogContentText>
                Now click the flap that is currently showing
            </DialogContentText>
            {
                Array.from(FLAPS).map((f) => <Button key={`button-${f}`} variant='outlined' onClick={() => {
                    goToFlap((FLAPS.length - FLAPS.indexOf(f)) % FLAPS.length)
                    setCalibrationStep(CalibrationStep.CALIBRATING)
                }}>{ f }</Button>)
            }
            </DialogContent>
        </>,
        [CalibrationStep.CALIBRATING]: () => 
            <>
            {
                module.moving ? (
                    <DialogContent>
                        <DialogContentText>
                            Calibrating, please wait...
                        </DialogContentText>
                    </DialogContent>
                    ) : (
                        <>
                    <DialogContent>
                        <DialogContentText>
                            Has the module calibrated to the home position?
                        </DialogContentText>
                    </DialogContent>
                    <DialogActions>
                        <Button variant='outlined' onClick={() => {
                            goToFlap(0);
                            setCalibrationStep(CalibrationStep.FIND_FLAP_BOUNDARY)
                        }}>Retry</Button>
                        <Button variant='contained' onClick={() => {
                            setOffsetToCurrentStep()
                            setDialogOpen(false)
                        }}>Done</Button>
                    </DialogActions>
                    </>
                    )
                }
            </>,

        [CalibrationStep.CONFIRM]: () => 
            <>
            <DialogContent>
                <DialogContentText>
                    Calibration complete!
                </DialogContentText>
            </DialogContent>
            <DialogActions>
                <Button variant='outlined' onClick={() => {
                    setCalibrationStep(CalibrationStep.FIND_FLAP_BOUNDARY)
                }}>Retry</Button>
                <Button variant='contained' onClick={() => {
                    setDialogOpen(false)
                }}>Done</Button>
            </DialogActions>
            </>,
    }

    const startCalibration = () => {
        goToFlap(0);
        setCalibrationStep(CalibrationStep.FIND_FLAP_BOUNDARY);
        setDialogOpen(true);
    }

    const onClick = (e: SyntheticEvent) => {
        console.log(e)
        if (e.type === 'click') {
            setSplitflapConfig((curConfig) => {
                return PB.SplitflapConfig.toObject(applyResetModule(PB.SplitflapConfig.create(curConfig), i), {
                    defaults: true,
                }) as NoUndefinedField<PB.ISplitflapConfig>
            })
        } else if (e.type === 'contextmenu') {
            e.preventDefault()
            startCalibration()
        }
    }
    return (
        <div
            key={`reset-${i}`}
            style={{
                fontSize: `${charWidth}px`,
                fontFamily: 'Roboto Mono',
                letterSpacing: `${0.4 * charWidth}px`,
                display: 'inline-block',
                width: `${charWidth}px`,
                cursor: 'pointer',
                border: '1px solid black',
            }}
        >
            <div
                onClick={onClick}
                onContextMenu={onClick}
            >
                <Tooltip title={
                    <div>
                        <div>State: {PB.SplitflapState.ModuleState.toObject(PB.SplitflapState.ModuleState.create(module), {
                            enums: String,
                        }).state}</div>
                        <div>Missed home: {module.countMissedHome}</div>
                        <div>Unexpected home: {module.countUnexpectedHome}</div>
                        <br />
                        <div><b>Click to reset module</b></div>
                    </div>
                }>
                    <div
                        style={{
                            width: '100%',
                            paddingLeft: module.state === PB.SplitflapState.ModuleState.State.LOOK_FOR_HOME ? 0 : `${charWidth*0.12}px`,
                            backgroundColor: module.state === PB.SplitflapState.ModuleState.State.SENSOR_ERROR ? 'orange' : 'inherit',
                            minWidth: module.state,
                            textAlign: 'center',
                        }}
                    >{
                        module.state === PB.SplitflapState.ModuleState.State.NORMAL ?
                        <>{FLAPS[module.flapIndex]}&nbsp;</> :
                        module.state === PB.SplitflapState.ModuleState.State.LOOK_FOR_HOME ?
                        <CircularProgress size={charWidth * 0.7} /> :
                        module.state === PB.SplitflapState.ModuleState.State.SENSOR_ERROR ?
                        <>&nbsp;</> :
                        'x'
                    }
                    </div>
                </Tooltip>
            </div>
            <Dialog open={dialogOpen} onClose={() => setDialogOpen(false)}>
                <DialogTitle>Calibrate module</DialogTitle>
                {calibrationComponent[calibrationStep]()}
            </Dialog>
        </div>
    )
}


type LogsProps = {
    logs: LogLine[],
    lastN: number,
    after?: Date,
}

const Logs: React.FC<LogsProps> = (props) => {
    return (
        <pre style={{
            fontSize: "1em",
            lineHeight: "1.2em",
            height: (props.lastN * 1.2) + "em"
        }}>{props.logs.slice(-props.lastN).filter(ll => props.after === undefined || ll[0] > props.after).map((ll => ll[0].toISOString() + ": " + ll[1])).join("\n")}</pre>
    )
}