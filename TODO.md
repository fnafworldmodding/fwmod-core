## NOTES
- may have to rework all chunks initializations, why do you init the value in
object constructor when it is done automatically by the compiler?? (talking about you chunk.id)

- instead of loading everything to memory, just ignore the unknown chunks completely
write them when it is time for the .dat to be modified

- duplicating memory is bad, having two copies of already existing data instead of
just using pointer like a normal dev (normal dev? I idk actually) or freeing old data

- try avoid copying/moving structs/objects when possible

## TODO
- decompiling/compiling
    - [x] reading/writing chunks
    - [x] AppHeader
    - #### Resources
        - [x] Image Offsets And Image Bank
        - [ ] Sound Offsets And Sound Bank
        - [ ] Font Offsets And Font Bank
        - [ ] Shaders Offsets And Shaders Bank
    - #### Objects
        - [x] Object headers
        - [ ] Object names
        - [x] Object properties
        - [ ] Object shaders
        - [x] Object Offsets
- API
    - [ ] ModStore (some kind of map that inform us what assets were successfully loaded)
    - [x] export global variables (CoreLogger, PluginsEventManager, etc)
    - [x] export BinaryReader and BinaryWriter classes
    - [x] export logger
    - [x] basic hooking functions
    - [ ] None at the moment (Planned)
- Asset Management
    - [ ] adding images/textures
    - [x] replacing images/textures
    - [ ] adding animations
    - [ ] replacing animations
    - [ ] adding shaders?
    - [ ] adding fonts
    - [ ] replacing fonts
    - [ ] adding sounds
    - [ ] replacing sounds
- Optimizations
    - [ ] lazy load chunks?
    - [ ] clear old data upon init
    - [ ] skip deserialization of resources/banks when not needed (times where nothing is modified or added)
    - [ ] caching system, instead of serializing data every single time upon launch, create a cache of that data (e.g images, animations)
    - [x] either lazy create bitdicts or don't use them at all. they get allocated to end up not being even used, just use basic math or macros (~~bitdict should only be wrappers over values~~ done they are now just wrappers)
- Other
    - [ ] remove pointless headers/code
    - [ ] clean up comments