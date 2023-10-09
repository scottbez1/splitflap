export const exhaustiveCheck = (x: never): never => {
    throw new Error("Didn't expect to get here", x)
}

export const isSome = <T>(v: T | null | undefined): v is T => {
    return v !== null && v !== undefined
}

export type NoUndefinedField<T> = {
    [P in keyof T]-?: NoUndefinedField<NonNullable<T[P]>>
}
