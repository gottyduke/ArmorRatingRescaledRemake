# ArmorRatingRescaled - Remake

## Build
* [CommonLibSSE](https://github.com/Ryan-rsm-McKenzie/CommonLibSSE)
* [DKUtil](https://github.com/gottyduke/DKUtil)

## End User Dependencies
* [SKSE64](https://skse.silverlock.org/)
* [Address Library](https://www.nexusmods.com/skyrimspecialedition/mods/32444)

## Settings
Setting | Type | Default | Description
--- | --- | --- | ---
`ArmorScalingFactor` | `float` | 1.0 | Multiplier for the vanilla damage resistance before it is used in the computation.
`DisableHiddenArmorRating` | `bool` | false | Set to true to disable the hidden armor rating.
`OverrideArmorCap` | `int` | 0 | Set the maximum damage resistance factor. A value of 100 equals no cap. When set to 0 the game setting fMaxArmorRating is used instead.