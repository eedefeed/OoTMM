export const PATCH_GROUPS = [
  'OOT_HOOKSHOT_ANYWHERE',
  'OOT_CLIMB_ANYWHERE',
  'MM_FD_ANYWHERE',
  'MM_HOOKSHOT_ANYWHERE',
  'MM_BLAST_MASK_DELAY_INSTANT',
  'MM_BLAST_MASK_DELAY_VERYSHORT',
  'MM_BLAST_MASK_DELAY_SHORT',
  'MM_BLAST_MASK_DELAY_LONG',
  'MM_BLAST_MASK_DELAY_VERYLONG',
  'MM_CLOCK_SPEED_VERYSLOW',
  'MM_CLOCK_SPEED_SLOW',
  'MM_CLOCK_SPEED_FAST',
  'MM_CLOCK_SPEED_VERYFAST',
  'MM_CLOCK_SPEED_SUPERFAST',
] as const;

export type PatchGroup = typeof PATCH_GROUPS[number];

export const PATCH_GROUP_VALUES: {[k: string]: number} = {};

for (let i = 0; i < PATCH_GROUPS.length; i++) {
  const k = PATCH_GROUPS[i];
  PATCH_GROUP_VALUES[k] = i + 1;
};
