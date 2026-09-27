# External models added to the workspace

All three assets are distributed by Poly Haven under CC0:
https://polyhaven.com/license

| Asset | Creator | Source |
|---|---|---|
| Potted Plant 01 | Rico Cilliers | https://polyhaven.com/a/potted_plant_01 |
| Wooden Bookshelf Worn | Ulan Cabanilla | https://polyhaven.com/a/wooden_bookshelf_worn |
| Desk Lamp Arm 01 | Kuutti Siitonen (modeling/texturing), Yann Kervran (rigging) | https://polyhaven.com/a/desk_lamp_arm_01 |

## Local conversion

Downloaded glTF geometry and 1K diffuse images from the official Poly Haven API.
Downloaded bytes are checked against the API's MD5 metadata. Source mesh, diffuse
images and download manifests remain in each model's source/ folder.

tools/import_polyhaven.py converts these assets to OBJ/MTL, bakes base-colour
factors into a diffuse texture atlas, applies node transforms and normals, and
normalises each model to height 1 with its base at Y=0. Source UVs are converted
from glTF to OBJ convention. The renderer sets their final sizes and positions.

These are static props. Source rigging, roughness, metalness and normal maps are
not used by the current Blinn-Phong renderer. This conversion does not reproduce
the complete PBR material appearance of the original assets.

This file covers only the three newly downloaded props. Existing person, desk,
monitor, mouse and keyboard assets retain their original provenance.

## Suggested Classroom credit

Additional models: Potted Plant 01 by Rico Cilliers; Wooden Bookshelf Worn by
Ulan Cabanilla; Desk Lamp Arm 01 by Kuutti Siitonen and Yann Kervran.
Downloaded from Poly Haven (CC0), source links above. Converted to OBJ and
diffuse atlases for this assignment. Include the original scene inspiration
and credits for the pair's existing assets separately.
