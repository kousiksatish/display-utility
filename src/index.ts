import bindings from 'bindings';
import lockSystem from 'lock-system';

/**
 * DisplayUtility will retrieve the information about the desktop display.
 * ScreenCaptureUtility will provide functions to capture screen and encode it.
 * Both these utilities are exposed by single .node file display-utility.node
 */
// tslint:disable-next-line: no-any
const addon: any = bindings('display-utility');

// tslint:disable-next-line: no-unsafe-any
export const displayUtility: INativeDisplayUtility = addon.DisplayUtility;

// Creating and exporting persistent object for screen capture utility
// tslint:disable-next-line: no-unsafe-any
export const screenCaptureUtility: IScreenCaptureUtility = new addon.ScreenCaptureUtility();

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
    setResolution(rROutput: number, resolution: IResolution): void;
    makeScreenBlank(): void;
    reverseBlankScreen(): void;
    getPrimaryRROutput(): number;
    getExtendedMonitorResolution(): IResolution | undefined;
}

interface ILockUtility {
    lockScreen(): void;
}

interface IScreenCaptureUtility {
    init(rROutput?: number): void;
    getNextFrame(callback: (nextFrame: ArrayBuffer) => void): void;
}
