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

/**
 * @export
 * @extends {IResolution}
 * height - height component of the resolution
 * width - width component of the resolution
 */
export interface IResolution {
    width: number;
    height: number;
}

/**
 * @export
 * @extends {IResolution}
 * offsetX - x offset of the output (monitor)
 * offsetY - y offset of the output (monitor)
 * rrOutput - rr output of the output (monitor)
 */
export interface IResolutionWithOffset extends IResolution {
    offsetX: number;
    offsetY: number;
    rrOutput: number;
}

interface INativeDisplayUtility {
    /**
     * Returns list of numbers which represents the RROutput corresponding to all the outputs
     * undefined returned in case of any error or no connected outputs
     */
    getConnectedOutputs(): number[] | undefined;

    /**
     * Returns the name of the output (monitor) given the RROutput number for that output
     */
    getOutputName(rROutput: number): string;

    /**
     * Returns the current resolution of the output (monitor) given the RROutput number for that output
     */
    getCurrentResolution(rROutput: number): IResolution | undefined;

    /**
     * Returns the list of possible resolutions for an output (montior) given the RROutput number for that output
     */
    getResolutions(rROutput: number): IResolution[] | undefined;

    /**
     * Sets a particular resolution for an output (monitor) given the RROutput number for that output and the new resolution to be set.
     */
    setResolution(rROutput: number, resolution: IResolution): void;

    /**
     * Reduces the brightness of all connected outputs to 0 and makes the screen blank.
     */
    makeScreenBlank(): void;

    /**
     * Reverses the operation performed by makeScreenBlank()
     */
    reverseBlankScreen(): void;

    /**
     * Get the RROutput number corresponding to the primary monitor
     * Note: When no monitor is marked as primary, one of the connected monitor's RROutput number is returned
     */
    getPrimaryRROutput(): number;

    /**
     * Get the complete resolution of the display when multiple monitors are connected.
     * Note: Returns same value as getCurrentResolution(), when single monitor is connected
     */
    getExtendedMonitorResolution(): IResolution | undefined;

    /**
     * Get the position (x-offset and y-offset), height and width of connected monitors
     */
    getAllCurrentResolutionsWithOffset(): IResolutionWithOffset[] | undefined;
}

interface ILockUtility {
    /**
     * Locks the linux computer when executed
     */
    lockScreen(): void;
}

interface IScreenCaptureUtility {
    /**
     * Initialises the screen capturer and x264 encoder for capturing screen
     */
    init(singleMonitorCapture: boolean, rROutput?: number): void;

    /**
     * Returns the captured and encoded frame buffer as a callback
     */
    getNextFrame(callback: (nextFrame: ArrayBuffer) => void): void;

    /**
     * Returns the captured and encoded frame buffer as a callback
     * Returns only if there is a change compared to previous captured frame.
     * In case of no changes, every 30ms we look for frame change and return next frame
     * If getIFrame is passed as true, next frame will be generated as IFrame
     */
    // getNextFrame(noChangeCheck: boolean, getIFrame: boolean, callback: (nextFrame: ArrayBuffer) => void): void;

    /**
     * Force callback from getNextFrame function if it is waiting for a change in frame
     */
    forceNextFrame(): void;

    /**
     * Force the next frame to be an iframe. Call will also force the next frame and return any waiting getNextFrame callback
     */
    sendNextFrameAsIFrame(): void;
}
