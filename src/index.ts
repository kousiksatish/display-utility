import bindings from 'bindings';

/**
 * DisplayUtility will retrieve the information about the desktop display.
 */
// tslint:disable-next-line: no-unsafe-any
export const displayUtility: INativeDisplayUtility = bindings('display-utility');

export interface IResolution {
    width: number;
    height: number;
}

export interface INativeDisplayUtility {
    getConnectedOutputs(): number[];
    getOutputName(outputIndex: number): string;
    getCurrentResolution(outputIndex: number): IResolution;
    getResolutions(outputIndex: number): IResolution[];
    setResolution(outputName: string, resolution: string): void;
    makeScreenBlank(): void;
    reverseBlankScreen(): void;
}
