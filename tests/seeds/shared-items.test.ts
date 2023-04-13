import { makeTestSeed } from "../helper";

test("Can make a seed - Shared Items", () => {
  makeTestSeed("SHARED-ITEMS", {
    sharedBows: true,
    sharedBombBags: true,
    sharedMagic: true,
    sharedMagicArrowFire: true,
    sharedMagicArrowIce: true,
    sharedMagicArrowLight: true,
    sharedSongs: true,
    sharedNutsSticks: true,
    sharedHookshot: true,
    sharedLens: true,
    sharedOcarina: true,
    sharedMasks: true,
    sharedWallets: true,
    sharedHealth: true,
  });
});
