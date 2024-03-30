import React, {useCallback, useEffect, useRef, useState} from 'react'
import Typography from '@mui/material/Typography'
import Container from '@mui/material/Container'
import {PB} from 'splitflapjs-proto'
import {AppBar, Button, Card, CardContent, CircularProgress, Link, Toolbar, Tooltip,} from '@mui/material'
import {NoUndefinedField} from './util'
import {SplitflapWebSerial} from 'splitflapjs-webserial'
import { applyResetModule } from 'splitflapjs-core/dist/util'

const FLAPS=' ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.  '
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

type MessageDelayMs = [string, number]

const IDLE_MESSAGES: MessageDelayMs[] = [
    ['SPLIT', 6000],
    [' FLAP', 6000],
    ['  BY', 6000],
    ['BEZEK', 6000],
    ['  LABS', 12000],
    ['', 2 * 60 * 1000],
]

export type AppProps = object
export const App: React.FC<AppProps> = () => {
    const [splitflap, setSplitflap] = useState<SplitflapWebSerial | null>(null)
    const [splitflapState, setSplitflapState] = useState<NoUndefinedField<PB.ISplitflapState>>(
        PB.SplitflapState.toObject(PB.SplitflapState.create(), {
            defaults: true,
        }) as NoUndefinedField<PB.ISplitflapState>,
    )
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
                        console.log('LOG from splitflap', message.log?.msg)
                    }
                })
                setSplitflap(splitflap)
                const loop = splitflap.openAndLoop()
                splitflap.sendConfig(PB.SplitflapConfig.create(splitflapConfig))
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

    const updateSplitflap = useCallback((value: string) => {
        // TODO: should probably change types and use applySetFlaps?
        setSplitflapConfig((cur) => {
            const newModules = []
            for (let i = 0; i < splitflapState.modules.length; i++) {
                const targetFlapIndex = value[i] !== undefined ? FLAPS.indexOf(value[i]) : 0
                newModules.push({
                    targetFlapIndex,
                    resetNonce: cur.modules[i]?.resetNonce ?? 0,
                    movementNonce: (cur.modules[i]?.movementNonce ?? 0) + (targetFlapIndex === 0 ? 0 : 1),
                })
            }
            return {
                modules: newModules,
            }
        })
    }, [splitflapState.modules])

    const idleTimeout = useRef<ReturnType<typeof setTimeout>>();
    const curMessage = useRef<number>(0);
    const nextMessage = () => {
        const m = IDLE_MESSAGES[(curMessage.current++) % IDLE_MESSAGES.length]
        updateSplitflap(m[0])
        idleTimeout.current = setTimeout(nextMessage, m[1])
    }
    useEffect(() => {
        if (!inputValue.user) {
            return
        }
        const t = idleTimeout.current
        if (t) {
            clearTimeout(t)
            curMessage.current = 0
        }
        idleTimeout.current = setTimeout(() => {
            setInputValue({val:'', user: false})
            nextMessage()
        }, 1 * 60 * 1000)
    }, [inputValue])

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
                            <Typography variant="h4" color="inherit">
                                Current state
                            </Typography>
                            <div style={{
                            }}>
                            {
                                splitflapState.modules.map((module, i) => {
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
                                            onClick={() => {
                                        setSplitflapConfig((curConfig) => {
                                            return PB.SplitflapConfig.toObject(applyResetModule(PB.SplitflapConfig.create(curConfig), i), {
                                                defaults: true,
                                            }) as NoUndefinedField<PB.ISplitflapConfig>
                                        })
                                    }}>
                                        <Tooltip title={
                                        <div>
                                            <div>State: {PB.SplitflapState.ModuleState.toObject(PB.SplitflapState.ModuleState.create(module), {
                                                enums: String,
                                            }).state}</div>
                                            <div>Missed home: {module.countMissedHome}</div>
                                            <div>Unexpected home: {module.countUnexpectedHome}</div>
                                            <br />
                                            <div><b>Click to reset module</b></div>
                                        </div>}>
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
                                    </div>)
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
