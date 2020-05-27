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

function testScreenCapture(): void {
    fs.writeFileSync('/tmp/output.h264', '');
    screenCaptureUtility.init(false);

    setInterval(() => {
        screenCaptureUtility.forceNextFrame();
    },          1000);

    // CRF change test
    setTimeout(() => {
        try {
            screenCaptureUtility.setCRFValue(28);
        } catch (err) {
            // tslint:disable-next-line: no-console
            console.log(`Error occured ${err} `);
        }
    },         3000);

    setTimeout(() => {
        screenCaptureUtility.init(true, 65);
     },        5000);
    processFrame();
}

function processFrame(): void {
    screenCaptureUtility.getNextFrame((frame: ArrayBuffer) => {
        // tslint:disable-next-line: no-console
        console.log(`Received ${frame.byteLength}`);
        fs.appendFileSync('/tmp/output.h264', new Buffer(frame));
        processFrame();
    });
}

// testScreenCapture();
