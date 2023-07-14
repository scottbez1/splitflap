import React, {useCallback, useEffect, useState} from 'react'
import Typography from '@mui/material/Typography'
import Container from '@mui/material/Container'
import {PB} from 'splitflapjs-proto'
import {Button} from '@mui/material'
import {NoUndefinedField} from './util'
import {SplitflapWebSerial} from 'splitflapjs-webserial'

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

export type AppProps = object
export const App: React.FC<AppProps> = () => {
    const [splitflap, setSplitflap] = useState<SplitflapWebSerial | null>(null)
    const [splitflapState, setSplitflapState] = useState<NoUndefinedField<PB.ISplitflapState>>(
        PB.SplitflapState.toObject(PB.SplitflapState.create(), {
            defaults: true,
        }) as NoUndefinedField<PB.ISplitflapState>,
    )
    const [inputValue, setInputValue] = useState('');

    const handleInputChange = (event: React.ChangeEvent<HTMLInputElement>) => {
      const { value } = event.target;
      const upper = value.toUpperCase()
      if (legalString(upper)) {
        setInputValue(upper);
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

    const updateSplitflap = useCallback(() => {
        setSplitflapConfig((cur) => {
            const newModules = []
            for (let i = 0; i < splitflapState.modules.length; i++) {
                console.log(inputValue[i])
                const targetFlapIndex = inputValue[i] !== undefined ? FLAPS.indexOf(inputValue[i]) : 0
                newModules.push({
                    targetFlapIndex,
                    resetNonce: 0,
                    movementNonce: (cur.modules[i]?.movementNonce ?? 0) + (targetFlapIndex === 0 ? 0 : 1),
                })
            }
            return {
                modules: newModules,
            }
        })
        setInputValue('')
    }, [inputValue, splitflapState.modules])

    return (
        <>
            <Container component="main" maxWidth="lg">
                    {splitflap !== null ? (
                        <>
                            <form onSubmit={(event) => {
                                    event.preventDefault()
                                    updateSplitflap()
                                }}>
                                    <div style={{
                                        marginTop: '200px',
                                        width: `${250 * splitflapState.modules.length}px`,
                                        overflow: 'hidden',
                                    }}>
                                        <div style={{
                                            left: 0,
                                            position: 'sticky'
                                        }}>
                                            <input 
                                                type="text"
                                                maxLength={splitflapState.modules.length}
                                                onChange={handleInputChange}
                                                value={inputValue}
                                                onBlur={e => e.target.focus()}
                                                spellCheck="false"
                                                style={{
                                                    caret: 'block',
                                                    paddingLeft: '42px',
                                                    paddingTop: '20px',
                                                    paddingBottom: '20px',
                                                    letterSpacing: '130px',
                                                    border: 0,
                                                    outline: 'none',
                                                    backgroundImage: 'url("/outline.svg")', 
                                                    backgroundSize: '250px',
                                                    backgroundRepeat: 'repeat-x',
                                                    backgroundPosition: '0px 18px',
                                                    fontSize: '200px',
                                                    fontFamily: 'Roboto Mono',
                                                    width: `${250 * splitflapState.modules.length + 50}px`,
                                                }}
                                                />
                                        </div>
                                    </div>
                            </form>
                        </>
                    ) : navigator.serial ? (
                        <Button onClick={connectToSerial} variant="contained">
                            Connect via Web Serial
                        </Button>
                    ) : (
                        <Typography>
                            Sorry, Web Serial API isn't available in your browser. Try the latest version of Chrome.
                        </Typography>
                    )}
            </Container>
        </>
    )
}
