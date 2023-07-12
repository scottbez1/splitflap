import React, {useEffect, useState} from 'react'
import Typography from '@mui/material/Typography'
import Container from '@mui/material/Container'
import {PB} from 'splitflapjs-proto'
import {Box, Button, CardActions, Paper} from '@mui/material'
import {NoUndefinedField} from './util'
import {SplitflapWebSerial} from 'splitflapjs-webserial'

// const FLAPS=' ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.#$'

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
    // eslint-disable-next-line @typescript-eslint/no-unused-vars
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

    return (
        <>
            <Container component="main" maxWidth="lg">
                <Paper variant="outlined" sx={{my: {xs: 3, md: 6}, p: {xs: 2, md: 3}}}>
                    <Typography component="h1" variant="h5">
                        Basic Splitflap Web Serial Demo
                    </Typography>
                    {splitflap !== null ? (
                        <>
                            <Box
                                component="form"
                                sx={{
                                    '& .MuiTextField-root': {m: 1, width: '25ch'},
                                }}
                                noValidate
                                autoComplete="off"
                                onSubmit={(event) => {
                                    event.preventDefault()
                                    setSplitflapConfig({
                                        modules: [
                                            {
                                                targetFlapIndex: Math.floor(Math.random()*39),
                                                resetNonce: 0,
                                                movementNonce: Math.floor(Math.random()*255),
                                            }
                                        ]
                                    })
                                }}
                            >
                                {/* <TextField
                                    label="Position"
                                    value={pendingSplitflapConfig.position}
                                    type="number"
                                    onChange={(event: React.ChangeEvent<HTMLInputElement>) => {
                                        setPendingSplitflapConfig((cur) => {
                                            return {
                                                ...cur,
                                                position: event.target.value,
                                            }
                                        })
                                    }}
                                /> */}
                                <br />
                                <Button type="submit" variant="contained">
                                    Apply
                                </Button>
                            </Box>
                            <pre>{JSON.stringify(splitflapState, undefined, 2)}</pre>
                        </>
                    ) : navigator.serial ? (
                        <CardActions>
                            <Button onClick={connectToSerial} variant="contained">
                                Connect via Web Serial
                            </Button>
                        </CardActions>
                    ) : (
                        <Typography>
                            Sorry, Web Serial API isn't available in your browser. Try the latest version of Chrome.
                        </Typography>
                    )}
                </Paper>
            </Container>
        </>
    )
}
