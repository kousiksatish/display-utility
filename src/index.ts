import bindings from 'bindings';
import lockSystem from 'lock-system';

/**
 * DisplayUtility will retrieve the information about the desktop display.
 */
// tslint:disable-next-line: no-unsafe-any
export const displayUtility: INativeDisplayUtility = bindings('display-utility');

export const lockUtility: ILockUtility = {
    lockScreen: lockSystem
};

export interface IResolution {
    width: number;
    height: number;
}

interface INativeDisplayUtility {
    getConnectedOutputs(): number[] | undefined;
    getOutputName(rROutput: number): string;
    getCurrentResolution(rROutput: number): IResolution | undefined;
    getResolutions(rROutput: number): IResolution[] | undefined;
    setResolution(rROutput: number, resolution: IResolution, makeScreenBlank?: boolean): void;
    makeScreenBlank(): void;
    reverseBlankScreen(): void;
    getPrimaryRROutput(): number;
}

interface ILockUtility {
    lockScreen(): void;
}
