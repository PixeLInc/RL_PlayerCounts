# Rocket league population counts

This plugin adds back rocket league population per-playlist and total counts to the main menu in a small overlay in the right hand corner of the screen.

The accuracy of these numbers are rough, since it's only fetched in certain situations (those of which I would still like to figure out.)
But it is a general overview and nice thing to have since Rocket League removed the counts.

**RLSDK is required to compile this plugin, It will not be provided. If you do not know how to obtain this, just use a pre-compiled DLL**

### Improvements that can be made
- Settings to allow moving the overlay and toggling it on and off without disabling the plugin
- Find and hook better events for re-fetching and initial fetch of the data.
- Figure out when this data is sent, or if we can fetch it on demand.

### Disclaimer
This is being provided as-is and there is no guarentee of any future updates for when the game updates or breaks things.
This code also may not be the best, I do not work with C++ very often.