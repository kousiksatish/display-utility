import fs from 'fs';
import { displayUtility, lockUtility, screenCaptureUtility } from './index';

// tslint:disable-next-line:no-console
console.log(displayUtility.getConnectedOutputs());
// lockUtility.lockScreen();

// tslint:disable-next-line:no-console
console.log(displayUtility.getPrimaryRROutput());
// displayUtility.setResolution(65, {
//     width: 1440,
//     height: 900
// });

// tslint:disable-next-line:no-console
console.log(displayUtility.getExtendedMonitorResolution());

// tslint:disable-next-line:no-console
console.log(displayUtility.getAllCurrentResolutionsWithOffset());

async function testScreenCapturer(): Promise<void> {
    try {
        fs.writeFileSync('/tmp/output.h264', '');
        screenCaptureUtility.init(true, 65);
        let numberOfFrames: number = 100;
        // tslint:disable-next-line: no-console
        console.time('fps');
        console.timeLog('fps');
        while (numberOfFrames !== 0) {
            screenCaptureUtility.getNextFrame((frame: ArrayBuffer) => {
                writeToFile(frame);
            });
            numberOfFrames = numberOfFrames - 1;
        }
        // tslint:disable-next-line: no-console
        console.timeEnd('fps');

        screenCaptureUtility.init(false);
        numberOfFrames = 100;
        // tslint:disable-next-line: no-console
        console.time('fps');
        console.timeLog('fps');
        while (numberOfFrames !== 0) {
            screenCaptureUtility.getNextFrame((frame: ArrayBuffer) => {
                writeToFile(frame);
            });
            numberOfFrames = numberOfFrames - 1;
        }
        // tslint:disable-next-line: no-console
        console.timeEnd('fps');
    } catch (err) {
        // tslint:disable-next-line: no-console
        console.log(err);
    }
}

function writeToFile(input: ArrayBuffer): void {
    fs.appendFileSync('/tmp/output.h264', new Buffer(input));
}

// tslint:disable-next-line: no-floating-promises
// testScreenCapturer();

function testForceFrame(): void {

    screenCaptureUtility.init(false);

    setInterval(() => {
        screenCaptureUtility.forceNextFrame();
    },          1000);

    setTimeout(() => {
        screenCaptureUtility.init(true, 65);
     },        5000);
    processFrame();
}

function processFrame(): void {
    screenCaptureUtility.getNextFrame((frame: ArrayBuffer) => {
        // tslint:disable-next-line: no-console
        console.log(`Received ${frame.byteLength}`);
        processFrame();
    });
}

// testForceFrame();

// tslint:disable-next-line: no-console
console.log(userSessionUtility.getUserSessionInfo());
