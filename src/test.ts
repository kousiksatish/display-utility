import fs from 'fs';
import { audioCaptureUtility, displayUtility, lockUtility } from './index';

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

// async function testScreenCapturer(): Promise<void> {
//     try {
//         fs.writeFileSync('/tmp/output.h264', '');
//         screenCaptureUtility.init(true, 65);
//         let numberOfFrames: number = 100;

//         while (numberOfFrames !== 0) {
//             screenCaptureUtility.getNextFrame((frame: ArrayBuffer) => {
//                 writeToFile(frame);
//             });
//             numberOfFrames = numberOfFrames - 1;
//         }

//         screenCaptureUtility.init(false);
//         numberOfFrames = 100;

//         while (numberOfFrames !== 0) {
//             screenCaptureUtility.getNextFrame((frame: ArrayBuffer) => {
//                 writeToFile(frame);
//             });
//             numberOfFrames = numberOfFrames - 1;
//         }

//     } catch (err) {
//         // tslint:disable-next-line: no-console
//         console.log(err);
//     }
// }

function testAudioCapture(): void {
    processAudioFrame();
}

function processAudioFrame(): void {
    audioCaptureUtility.getNextFrame((frame: ArrayBuffer) => {
        // tslint:disable-next-line: no-console
        console.log(`Received ${frame.byteLength}`);
        writeToFile(frame);
        processAudioFrame();
    });
}

testAudioCapture();

function writeToFile(input: ArrayBuffer): void {
    fs.appendFileSync('/tmp/output.mp4', new Buffer(input));
}

// function testForceFrame(): void {

//     screenCaptureUtility.init(false);

//     setInterval(() => {
//         screenCaptureUtility.forceNextFrame();
//     },          1000);

//     setTimeout(() => {
//         screenCaptureUtility.init(true, 65);
//      },        5000);
//     processFrame();
// }

// function processFrame(): void {
//     screenCaptureUtility.getNextFrame((frame: ArrayBuffer) => {
//         // tslint:disable-next-line: no-console
//         console.log(`Received ${frame.byteLength}`);
//         processFrame();
//     });
// }

// function testScreenCaptureCrfValue(): void {
//     fs.writeFileSync('/tmp/output.h264', '');
//     screenCaptureUtility.init(false);

//     setInterval(() => {
//         screenCaptureUtility.forceNextFrame();
//     },          1000);

//     // CRF change test
//     setTimeout(() => {
//         try {
//             screenCaptureUtility.setCRFValue(28);
//         } catch (err) {
//             // tslint:disable-next-line: no-console
//             console.log(`Error occured ${err} `);
//         }
//     },         3000);

//     setTimeout(() => {
//         screenCaptureUtility.init(true, 65);
//      },        5000);
//     processFrame();
// }

// tslint:disable-next-line: no-floating-promises
// testScreenCapturer();
// testForceFrame();
// testScreenCaptureCrfValue();
