import { PB } from "splitflapjs-proto"


export const applySetFlaps = (config: PB.SplitflapConfig, positions: Array<number | null>, forceMovement?: Array<boolean>): PB.SplitflapConfig => {
    if (positions.length > config.modules.length) {
        console.warn(`Ignoring applySetFlaps: mismatch in positions specified (${positions.length}) vs config (${config.modules.length})!`)
        return config
    }
    if (forceMovement !== undefined && forceMovement.length != positions.length) {
        throw new Error(`forceMovement (${forceMovement.length}) and positions (${positions.length}) length mismatch!`)
    }

    const newConfig = PB.SplitflapConfig.create(config)
    for (let i = 0; i < positions.length; i++) {
        if (positions[i] !== null) {
            newConfig.modules[i].targetFlapIndex = positions[i]
        }
        if (forceMovement !== undefined && forceMovement[i]) {
            newConfig.modules[i].movementNonce = ((newConfig.modules[i].movementNonce || 0) + 1) % 256
        }
    }
    return newConfig
}

export const applyResetModules = (config: PB.SplitflapConfig, positions: boolean[]): PB.SplitflapConfig => {
    if (positions.length > config.modules.length) {
        console.warn(`Ignoring applyResetModules: mismatch in positions specified (${positions.length}) vs config (${config.modules.length})!`)
        return config
    }

    const newConfig = PB.SplitflapConfig.create(config)
    for (let i = 0; i < positions.length; i++) {
        if (positions[i]) {
            newConfig.modules[i].resetNonce = ((newConfig.modules[i].resetNonce || 0) + 1) % 256
        }
    }
    return newConfig
}

export const applyResetModule = (config: PB.SplitflapConfig, position: number): PB.SplitflapConfig => {
    const reset = new Array(position + 1)
    reset.fill(false)
    reset[position] = true
    return applyResetModules(config, reset)
}

export function convert2dDualRowZigZagTo1dChainlink<T>(arr: T[][], flipFirstRows = false): T[] {
    const cols = arr[0].length
    const rows = arr.length

    const out: T[] = new Array<T>(cols * rows)
    for (let i = 0; i < cols * rows; i++) {
        const rowPair = Math.floor(Math.floor(i / 2) / cols)
        const upsideDownRowPair = (rowPair + (flipFirstRows ? 1 : 0)) % 2 !== 0

        const rowBase = rowPair * 2
        const rowOffset = upsideDownRowPair ? i % 2 : 1 - (i % 2)

        const row = rowBase + rowOffset
        const col = upsideDownRowPair ? cols - 1 - (Math.floor(i / 2) % cols) : Math.floor(i / 2) % cols

        out[i] = arr[row][col]
    }
    return out
}

export function convert1dChainlinkTo2dDualRowZigZag<T>(arr: T[], cols: number, flipFirstRows = false): T[][] {
    const rows = Math.ceil(arr.length / cols)

    const out: T[][] = new Array<T[]>(rows)
    for (let i = 0; i < rows; i++) {
        out[i] = new Array<T>(cols)
    }

    for (let i = 0; i < arr.length; i++) {
        const rowPair = Math.floor(Math.floor(i / 2) / cols)
        const upsideDownRowPair = (rowPair + (flipFirstRows ? 1 : 0)) % 2 !== 0

        const rowBase = rowPair * 2
        const rowOffset = upsideDownRowPair ? i % 2 : 1 - (i % 2)

        const row = rowBase + rowOffset
        const col = upsideDownRowPair ? cols - 1 - (Math.floor(i / 2) % cols) : Math.floor(i / 2) % cols

        out[row][col] = arr[i]
    }
    return out
}
