# Real Time Water Sim

## Setup

### MacOS

#### Resources
https://github.com/KhronosGroup/MoltenVK

#### Download and setup repo
1. Change to directory where you would like to download repo
    - NOTE: I would create a new folder under /Users/username.  The default Documents folder gave me a lot of trouble with permissions, do not put the repo inside any of the default Mac provided folders.
2. `git clone git@github.com:IshanRanade/RealTimeWaterSim.git`
3. `cd RealTimeWaterSim`

#### Initial setup
1. Download Xcode
2. Download the vulkan sdk from online, it should download as a zip.  Unzip the file to any directory.
    - NOTE: I would place this sdk folder directly inside your repo, not in any default Mac folder such as Desktop or Documents, this will save you issues with permissions.
3. Set the environment variable VULKAN_SDK=/pathtodownloadedsdk/macOS: <br/>
`export VULKAN_SDK=/pathtodownloadedvulkansdkunzippedcontents/macOS` <br/>
    - NOTE: You need to run this command in the terminal window that you use to run cmake so the terminal knows this variable.
4. `xcode-select --install`



#### Run with command line
1. cd into the root of the repo
2. `mkdir build`
3. `cd build`
4. `cmake ..`
5. `make`
6. Set the following environment variables through the command line in the same terminal you will use to run the program. <br/>
`export VK_ICD_FILENAMES = /pathtovulkansdk/macOS/etc/vulkan/icd.d/MoltenVK_icd.json` <br/>
`export VK_LAYER_PATH = /pathtovulkansdk/macOS/etc/vulkan/explicit_layer.d`
7. `./RealTimeWaterSim`

#### Run through Xcode
https://vulkan.lunarg.com/doc/sdk/1.1.92.1/mac/getting_started.html
1. cd into the root of the repo
2. `mkdir build`
3. `cd build`
4. `cmake -G Xcode ..`
5. Open Xcode project
6. Set the correct Scheme at the top to be RealTimeWaterSim > My Mac
7. Press Command + Shift + <, this allows you to edit the Scheme for this project.  Add to the arguments tab the following environment variables: <br/> 
VK_ICD_FILENAMES = /pathtosdk/macOS/etc/vulkan/icd.d/MoltenVK_icd.json <br/>
VK_LAYER_PATH = /pathtosdk/macOS/etc/vulkan/explicit_layer.d
8. Now, you have to allow Xcode to have Full Disk Access before attempting to build.  Go to System Preferences > Security & Privacy > Privacy, then click on Full Disk Access, then check Xcode in the box to the right, and save settings.  Restart Xcode after this step.
9. Press Play button, run normally through Xcode