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
| City tower | Skyline Vista Tower | Abbos Mirzaev | https://www.blendkit.com/asset-gallery-detail/ff2c8a98-a0de-4187-ad16-871fc227a71c/ |
| City tower | Urban Pulse Tower | Abbos Mirzaev | https://www.blendkit.com/asset-gallery-detail/cf4883e9-1cbf-4ee9-9831-e390ac7a6c28/ |
| City building | Short Suburban Office Building | Vox Quintinious | https://www.blendkit.com/asset-gallery-detail/1af3a396-cb5d-4baf-adfe-ffacc485f27d/ |
| Curtains | Simple curtains with rod | ydd 3D | https://www.blendkit.com/asset-gallery-detail/420772bc-beac-40db-95a5-833a4e5be799/ |
| Wall shelf | Hanging Wall Shelf | Patryk Zemsta | https://www.blendkit.com/asset-gallery-detail/6eeaa818-6f8c-41d6-a2d7-cce823ea1f8d/ |
| Books | Assorted of Design Book Set | Vicky Nguyen | https://www.blendkit.com/asset-gallery-detail/23e38383-2144-4bb3-81a3-e1bbede1aade/ |
| Wall art | Modern Geometric Wall Art Poster | Visual Diverse | https://www.blendkit.com/asset-gallery-detail/cd501baf-6fab-4a62-9af8-2d681ab18287/ |

These assets were converted from BlendKit glTF into OBJ, MTL and diffuse
textures for the assignment's renderer. Source PBR materials and rigging are
not reproduced. See [BlendKit license](https://www.blendkit.com/docs/licenses/)
and [terms](https://www.blendkit.com/terms-and-conditions-2018/) before sharing
model files. They are ignored by Git to avoid republishing standalone assets
in the public repository.

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

The active curtains are the user's `Models/curtain/Untitled.obj` export
(`Curtain_Right` and `Curtain_Left`). Its 15,150 quad faces were split into
30,300 triangles in `Models/curtain/curtain.obj`. Panels are gathered to the
sides by compressing their width and translating them; UVs are retained and
normals are transformed to match the new geometry. The original export is retained.
The renderer uses `Fabric_Smooth_Gray`'s exported `Kd` colour (0.8, 0.8, 0.8).
No diffuse texture was exported; `C:/Fabric036_2K_NormalGL.jpg` is an external
normal-map reference, not a supplied diffuse image. Source URL, creator and
license still need to be recorded from the model's original asset page.
