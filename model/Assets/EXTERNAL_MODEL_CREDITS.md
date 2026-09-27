# External model credits

The project includes these free BlendKit models under the site's
Royalty Free license. The person, desk, monitors, keyboard and mouse are the pair's
original models and were not replaced.

| Scene use | Asset | Creator | Source |
|---|---|---|---|
| Plant | Indoor Plant - Cuban Cigar | Pandora Land | https://www.blendkit.com/asset-gallery-detail/e942cd91-5350-4aa3-9c4b-8e75cf3b3cf6/ |
| Desk lamp | Minimalist LED Desk Lamp white | Demycs | https://www.blendkit.com/asset-gallery-detail/263afdd9-ce10-4c44-8626-45a510b78ac5/ |
| Bed | Gray box spring bed | Joyble | https://www.blendkit.com/asset-gallery-detail/8bf9e517-a0f8-4969-a2a2-49639eed73a1/ |
| Side table | Oak round End table | Joris LM | https://www.blendkit.com/asset-gallery-detail/76cc1ce7-f208-4b81-a489-4d12e1f54889/ |
| Wall clock | Office Wall Clock | Demycs | https://www.blendkit.com/asset-gallery-detail/d9cb7b80-4ca3-422e-9f69-26ca86a31507/ |
| City tower | Modern Glass Commercial Skyscraper | Vox Quintinious | https://www.blendkit.com/asset-gallery-detail/b35eb361-2426-40a9-8f4a-baa4c56ca638/ |
| City building | Office Building | Abraham Ballesteros | https://www.blendkit.com/asset-gallery-detail/1fa12b17-c77e-4ad8-99cc-b5428776eccd/ |
| City tower | Highrise Building | ReWork3D | https://www.blendkit.com/asset-gallery-detail/da0526ef-d5f6-4460-8ff5-1c5ac2fe096d/ |
| Curtains | Simple curtains with rod | ydd 3D | https://www.blendkit.com/asset-gallery-detail/420772bc-beac-40db-95a5-833a4e5be799/ |
| Wall shelf | Hanging Wall Shelf | Patryk Zemsta | https://www.blendkit.com/asset-gallery-detail/6eeaa818-6f8c-41d6-a2d7-cce823ea1f8d/ |
| Books | Assorted of Design Book Set | Vicky Nguyen | https://www.blendkit.com/asset-gallery-detail/23e38383-2144-4bb3-81a3-e1bbede1aade/ |
| Wall art | Modern Geometric Wall Art Poster | Visual Diverse | https://www.blendkit.com/asset-gallery-detail/cd501baf-6fab-4a62-9af8-2d681ab18287/ |

These assets were converted from BlendKit glTF into OBJ, MTL and diffuse
textures for the assignment's renderer. Source PBR materials and rigging are
not reproduced. See [BlendKit license](https://www.blendkit.com/docs/licenses/)
and [terms](https://www.blendkit.com/terms-and-conditions-2018/) before sharing
model files. Runtime assets are tracked with this assignment at the project
owner's request; inclusion does not change their original licenses.

The three active city models contain 65,000, 55,338 and 65,000 triangular
faces respectively. The glass skyscraper and highrise were decimated for
real-time rendering. Source base-colour UV sets were preserved in a diffuse
atlas. Several source GLB materials contain black baked images; those facades
use a replacement slate-blue material with an alpha facade mask. The city-only
shader adds procedural window lights, moonlight and distance haze, not source
PBR materials. Eight foreground and eighteen background placements reuse these
three models. The previous Skyline Vista, Urban Pulse and Short Suburban Office
models are no longer rendered.

The wall shelf, book sets and plant assets are reused for the headboard
bookshelf. A round end table supports the left reading lamp and small plant.
The new props were converted using Blender's glTF importer and triangulated
into 2,248 (table) and 568 (clock) faces, with diffuse texture atlases. The
clock's transparent glass cover is omitted because this renderer uses opaque
diffuse materials; the face, hands and frame are retained.
The old BlendKit curtains, floor-mounted wall shelf, wall art and
tall bookshelf remain removed from the scene.

The previous user-provided floor bookshelf was `Models/Untitled.obj`
(`Bookshelf_Light`, exported from Blender 5.2.2). All 2,160 faces are triangles.
Its source URL, creator and license were not included with the export and
must be added to the submission credits when known. The supplied MTL has
no diffuse image; it was rendered with its `Kd` colour (0.8, 0.8, 0.8).
The referenced `C:/Light Wood_Normal.exr` was not supplied.

The active curtains came from the user's `Models/curtain/Untitled.obj` export
(`Curtain_Right` and `Curtain_Left`). Its 15,150 quad faces were split into
30,300 triangles in `Models/curtain/curtain.obj`. Panels are gathered to the
sides by compressing their width and translating them; UVs are retained and
normals are transformed to match the new geometry. Only the triangulated OBJ
and its required `Untitled.mtl` are retained in the project; the original export
is recoverable from Git history.
The renderer uses `Fabric_Smooth_Gray`'s exported `Kd` colour (0.8, 0.8, 0.8).
No diffuse texture was exported; `C:/Fabric036_2K_NormalGL.jpg` is an external
normal-map reference, not a supplied diffuse image. Source URL, creator and
license still need to be recorded from the model's original asset page.
