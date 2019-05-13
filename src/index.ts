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
    getConnectedOutputs(): number[];
    getOutputName(rROutput: number): string;
    getCurrentResolution(rROutput: number): IResolution;
    getResolutions(rROutput: number): IResolution[];
    setResolution(rROutput: number, resolution: IResolution): void;
    makeScreenBlank(): void;
    reverseBlankScreen(): void;
    getPrimaryRROutput(): number;
}

interface ILockUtility {
    lockScreen(): void;
}
