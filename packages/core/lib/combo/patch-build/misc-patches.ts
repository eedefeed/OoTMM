import { Patchfile } from "./patchfile";

// Surprisingly both commented sections set all the fish to weigh 0 pounds.
// Unsure what's going on, but they're both in OOTR for fishing speedup
export function fishingSpeedups(patch: Patchfile) {
    const blockCode = [
        0x3c, 0x0A, 0x80, 0x12, 0x8D, 0x4A, 0xA5, 0xD4, 0x14, 0x0a, 0x00, 0x06,
        0x31, 0x78, 0x00, 0x01, 0x14, 0x18, 0x00, 0x02, 0x3c, 0x18, 0x42, 0x30,
        0x3c, 0x18, 0x42, 0x50, 0x03, 0xe0, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00,
        0x14, 0x18, 0x00, 0x02, 0x3c, 0x18, 0x42, 0x10, 0x3c, 0x18, 0x42, 0x38,
        0x03, 0xe0, 0x00, 0x08
    ]
    // patch.addPatch('oot', 0xDBF434, Buffer.from([0x44, 0x98, 0x90, 0x00, 0xe6, 0x52, 0x01, 0x9C]))
    patch.addPatch('oot', 0x3480C00, Buffer.from(blockCode))
    patch.addPatch('oot', 0xDBF484, Buffer.alloc(4))
    patch.addPatch('oot', 0xDBF4A8, Buffer.alloc(4))
    patch.addPatch('oot', 0xDCBEAA, Buffer.from([0x42, 0x48])) // set adult fish size requirement
    patch.addPatch('oot', 0xDCBF26, Buffer.from([0x42, 0x48])) // set adult fish size requirement
    patch.addPatch('oot', 0xDCBF32, Buffer.from([0x42, 0x30])) // set child fish size requirement
    patch.addPatch('oot', 0xDCBF9E, Buffer.from([0x42, 0x30])) // set child fish size requirement
    // patch.addPatch('oot', 0xDBF428, Buffer.from([0x0C, 0x10, 0x03, 0x00])) // Set Fishing Hook
}

export function easyFishingWithSinkingLure(patch: Patchfile) {
    /* Make sinking lure available immediately */
    patch.addPatch('oot', 0xDC2F00, Buffer.alloc(4))
    patch.addPatch('oot', 0xDC2F10, Buffer.alloc(4))
    /* Don't set sinking lure position after recieving child/adult fishing prizes */
    patch.addPatch('oot', 0xDCC064, Buffer.alloc(4))
    patch.addPatch('oot', 0xDCC06C, Buffer.alloc(4))
    patch.addPatch('oot', 0xDCC12C, Buffer.alloc(4))
    patch.addPatch('oot', 0xDCC134, Buffer.alloc(4))
    /* Give the child/adult fishing prizes even if using the sinking lure */
    patch.addPatch('oot', 0xDCBEBC, Buffer.alloc(4))
    patch.addPatch('oot', 0xDCBEC0, Buffer.alloc(4))
    patch.addPatch('oot', 0xDCBF1C, Buffer.alloc(4))
    patch.addPatch('oot', 0xDCBF20, Buffer.alloc(4))
    /* # Display the normal text when getting the prize, instead of text saying the sinking lure is in violation of the rules */
    patch.addPatch('oot', 0xDCBBDB, Buffer.from([0x86]))

    /* In case 1: of Fishing_UpdateFish, set unk_1A2 = 200 instead of 20000 */
    let b: Buffer = Buffer.alloc(4)
    b.writeUInt32BE(0x240100c8)
    patch.addPatch('oot', 0xDC652C, b) // replace 'mtc1 zero, f10' with 'addiu at, zero, 0x00c8'
    b.writeUInt32BE(0xa6010192)
    patch.addPatch('oot', 0xDC6540, b) // replace 'sh v0, 0x0192(s0)' with 'sh at, 0x0192(s0)'
    b.writeUInt32BE(0xE60601AC)
    patch.addPatch('oot', 0xDC6550, b) // replace 'swc1 f10, 0x01ac(s0)' with 'swc1 f6, 0x01ac(s0)'
}
