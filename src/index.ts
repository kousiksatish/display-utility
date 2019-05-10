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
    getOutputName(outputIndex: number): string;
    getCurrentResolution(outputIndex: number): IResolution;
    getResolutions(outputIndex: number): IResolution[];
    setResolution(outputName: string, resolution: string): void;
    makeScreenBlank(): void;
    reverseBlankScreen(): void;
}

interface ILockUtility {
    lockScreen(): void;
}
