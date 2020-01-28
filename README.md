# Real Time Water Sim

## Setup

### MacOS

#### Resources
https://github.com/KhronosGroup/MoltenVK

#### Initial setup
1. Download Xcode
2. Download the vulkan sdk from online, it should download as a zip.  Unzip the file to any directory.
3. Set the environment variable VULKAN_SDK=/pathtodownloadedsdk/macOS: <br/>
`export VULKAN_SDK=/pathtodownloadedvulkansdkunzippedcontents/macOS` <br/>
You need to run this command in the terminal window that you use to run cmake so the terminal knows this variable.
4. `xcode-select --install`

#### Download and setup repo
1. Change to directory where you would like to download repo
2. `git clone git@github.com:IshanRanade/RealTimeWaterSim.git`
3. `cd RealTimeWaterSim`
5. `mkdir build`
6. `cd build`

#### Run with command line
1. cd into the build folder located in the top level of the repo, create one if it does not exist
2. `cmake ..`
3. `make`
4. Set the following environment variables through the command line in the same terminal you will use to run the program. <br/>
`export VK_ICD_FILENAMES = /pathtosdk/macOS/etc/vulkan/icd.d/MoltenVK_icd.json` <br/>
`export VK_LAYER_PATH = /pathtosdk/macOS/etc/vulkan/explicit_layer.d`
5. `./RealTimeWaterSim`

#### Run through Xcode
https://vulkan.lunarg.com/doc/sdk/1.1.92.1/mac/getting_started.html
1. cd into the build folder located in the top level of the repo, create one if it does not exist
2. `cmake -G Xcode ..`
3. Open Xcode project
4. Press Command + Shift + <, this allows you to edit the Scheme for this project.  Add to the arguments tab the following environment variables: <br/> 
VK_ICD_FILENAMES = /pathtosdk/macOS/etc/vulkan/icd.d/MoltenVK_icd.json <br/>
VK_LAYER_PATH = /pathtosdk/macOS/etc/vulkan/explicit_layer.d
5. Set the correct Scheme at the top to be RealTimeWaterSim > My Mac
6. Now, you have to allow Xcode to have Full Disk Access before attempting to build.  Go to System Preferences > Security & Privacy > Privacy, then click on Full Disk Access, then check Xcode in the box to the right, and save settings.  Restart Xcode after this step.
6. Press Play button, run normally through Xcode