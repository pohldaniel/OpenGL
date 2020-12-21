# OpenGL_Projects

Move the camera with 'w' 'a' 's' 'd' 'q' 'e' and mouse, release the capture with space. Activate lightspot with '1' and '2'.

I used tinyxml https://github.com/vmayoral/tinyxml, parts of stb https://github.com/nothings/stb in particular stb_image.c, glm https://glm.g-truc.net/0.9.9/index.html and parts of https://github.com/emargollo/OpenGL-Animator for the project 13ColladaViewer-animation and do not own the copyright. The cowboy model is from "ThinMatrix" https://github.com/TheThinMatrix/OpenGL-Animation and the dragon model from "Imphenzia" https://www.youtube.com/watch?v=otiEQZr2idM I also do not own the copyright.

Output from 13ColladaViewer-animation

![Collada](https://user-images.githubusercontent.com/30089026/95699071-d45ae180-0c43-11eb-91e4-ad597412f37f.jpg)

The next image shows my export options: To avoid the use of the CORRECTION Matrix a change of the Y and Z axis is neccesary. Furthermore renaming the armature to "Armature", shown in the blue rectangle, is neccesary. Otherwise this simple parser will crash. I also add the .blend files, they represent some special cases. At mushroom.blend the "bone chain" of the animation is holding a gap. At dragon.blend the "bone.chain" doesn't start with the root. At the moment I ignore the weights and there is still a lot of cleaning up to do :).

![Collada](https://user-images.githubusercontent.com/30089026/95698007-e0916f80-0c40-11eb-8fb8-9ec466681a0b.jpg)

Output from 07OBJViewer-normalMap

![unbenannt1](https://user-images.githubusercontent.com/30089026/43673297-cea307bc-97c0-11e8-8418-d84cd3f4a1b3.jpg)


Output from 08OBJViewer-relief
![relief](https://user-images.githubusercontent.com/30089026/50221719-63737700-0396-11e9-90e0-15082469de94.jpg)

I expand the sss shader form https://github.com/jhaberstro/subsurface-scattering for the project 10OBJViewer-subsurface-scattering and do not own the copyright. In particular I add projective texture mapping to use the shader in relation with perspective projection. Output:

![Subsurface](https://user-images.githubusercontent.com/30089026/54387008-9332d200-469a-11e9-81ea-933f0d85d670.jpg)

The project 11OBJViewer-translucency is a refactoring of https://github.com/tatsy/FastTranslucentShader

