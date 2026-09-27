"""Download CC0 Poly Haven props and convert their glTF meshes to textured OBJ.

Requires numpy and Pillow. Original downloads and metadata are retained.
Coordinates are normalised to a one-metre height, centred on X/Z, grounded at Y=0.
Multiple source materials are baked into one diffuse atlas for the current loader.
"""
import hashlib
import json
from pathlib import Path
import urllib.request
import numpy as np
from PIL import Image

ROOT = Path(__file__).resolve().parents[1] / "model/Assets/Models"
ASSETS = ("potted_plant_01", "wooden_bookshelf_worn", "desk_lamp_arm_01")

def fetch(url):
    if not url.startswith(("https://api.polyhaven.com/", "https://dl.polyhaven.org/")):
        raise ValueError("Unexpected source URL")
    with urllib.request.urlopen(urllib.request.Request(url, headers={"User-Agent":"Assignment2-AssetImporter/1.0"}), timeout=120) as response:
        return response.read()

def download(entry, path):
    path.parent.mkdir(parents=True, exist_ok=True)
    data = path.read_bytes() if path.exists() else fetch(entry["url"])
    if hashlib.md5(data).hexdigest() != entry["md5"]:
        raise ValueError("Checksum mismatch: " + str(path))
    path.write_bytes(data)

def convert(folder, asset):
    source = folder / "source"
    doc = json.loads((source / (asset + "_1k.gltf")).read_text())
    buffers = [(source / b["uri"]).read_bytes() for b in doc["buffers"]]
    types = {5121:np.uint8,5123:np.dtype("<u2"),5125:np.dtype("<u4"),5126:np.dtype("<f4")}
    widths = {"SCALAR":1,"VEC2":2,"VEC3":3,"VEC4":4}
    def accessor(index):
        a=doc["accessors"][index]
        if "sparse" in a: raise ValueError("Sparse accessor unsupported")
        v=doc["bufferViews"][a["bufferView"]]
        dtype=np.dtype(types[a["componentType"]]); n=widths[a["type"]]
        return np.ndarray((a["count"],n),dtype,buffer=buffers[v["buffer"]],
            offset=v.get("byteOffset",0)+a.get("byteOffset",0),
            strides=(v.get("byteStride",n*dtype.itemsize),dtype.itemsize)).copy()

    materials=doc.get("materials", [{}])
    tile=1024
    atlas=Image.new("RGB",(tile*len(materials),tile))
    for i,m in enumerate(materials):
        pbr=m.get("pbrMetallicRoughness",{})
        factor=np.array(pbr.get("baseColorFactor",[1,1,1,1])[:3])
        tex=pbr.get("baseColorTexture")
        if tex:
            imageIndex=doc["textures"][tex["index"]]["source"]
            pic=Image.open(source/doc["images"][imageIndex]["uri"]).convert("RGB").resize((tile,tile))
        else: pic=Image.new("RGB",(tile,tile),"white")
        pic=Image.fromarray(np.uint8(np.clip(np.array(pic)*factor,0,255)))
        atlas.paste(pic,(i*tile,0))
    atlas.save(folder/(asset+".jpg"),quality=94)

    batches=[]
    def visit(index,parent):
        node=doc["nodes"][index]
        if "matrix" in node:
            local=np.array(node["matrix"]).reshape(4,4,order="F")
        else:
            x,y,z,w=node.get("rotation",[0,0,0,1])
            rot=np.array([[1-2*(y*y+z*z),2*(x*y-z*w),2*(x*z+y*w)],
                [2*(x*y+z*w),1-2*(x*x+z*z),2*(y*z-x*w)],
                [2*(x*z-y*w),2*(y*z+x*w),1-2*(x*x+y*y)]])
            local=np.eye(4); local[:3,:3]=rot@np.diag(node.get("scale",[1,1,1]))
            local[:3,3]=node.get("translation",[0,0,0])
        world=parent@local
        if "mesh" in node:
            for p in doc["meshes"][node["mesh"]]["primitives"]:
                if p.get("mode",4)!=4: raise ValueError("Non-triangle primitive")
                attrs=p["attributes"]
                positions=accessor(attrs["POSITION"]).astype(float)
                positions=positions@world[:3,:3].T+world[:3,3]
                uv=accessor(attrs["TEXCOORD_0"]).astype(float)
                if np.any(uv < -0.001) or np.any(uv > 1.001):
                    raise ValueError("Repeating UVs need a different atlas conversion")
                uv[:,0]=(np.clip(uv[:,0],0.001,0.999)+p.get("material",0))/len(materials)
                uv[:,1]=1-np.clip(uv[:,1],0.001,0.999)
                normals=accessor(attrs["NORMAL"]).astype(float)@np.linalg.inv(world[:3,:3])
                normals/=np.maximum(np.linalg.norm(normals,axis=1,keepdims=True),1e-12)
                indices=accessor(p["indices"]).reshape(-1,3)
                if np.linalg.det(world[:3,:3])<0: indices=indices[:,::-1]
                batches.append((positions,uv,normals,indices))
        for child in node.get("children",[]): visit(child,world)
    for node in doc["scenes"][doc.get("scene",0)]["nodes"]: visit(node,np.eye(4))
    allpos=np.concatenate([b[0] for b in batches])
    lo=allpos.min(axis=0); hi=allpos.max(axis=0)
    origin=np.array([(lo[0]+hi[0])/2,lo[1],(lo[2]+hi[2])/2])
    height=hi[1]-lo[1]
    with (folder/(asset+".obj")).open("w") as obj:
        obj.write("# Converted from Poly Haven CC0 asset: "+asset+"\nmtllib "+asset+".mtl\nusemtl atlas\n")
        offset=1
        for positions,uv,normals,indices in batches:
            for p in (positions-origin)/height: obj.write("v %.7f %.7f %.7f\n"%tuple(p))
            for t in uv: obj.write("vt %.7f %.7f\n"%tuple(t))
            for n in normals: obj.write("vn %.7f %.7f %.7f\n"%tuple(n))
            for face in indices+offset:
                obj.write("f "+" ".join(f"{int(i)}/{int(i)}/{int(i)}" for i in face)+"\n")
            offset+=len(positions)
    (folder/(asset+".mtl")).write_text("newmtl atlas\nKd 1 1 1\nmap_Kd "+asset+".jpg\n")
    print(asset, "triangles",sum(len(b[3]) for b in batches),"original bounds",lo,hi,flush=True)

for asset in ASSETS:
    folder=ROOT/asset; source=folder/"source"; source.mkdir(parents=True,exist_ok=True)
    files=json.loads(fetch("https://api.polyhaven.com/files/"+asset))
    entry=files["gltf"]["1k"]["gltf"]
    download(entry,source/(asset+"_1k.gltf"))
    for name,item in entry["include"].items():
        if name.endswith(".bin") or "_diff_" in name:
            target=(source/name).resolve()
            if not target.is_relative_to(source.resolve()): raise ValueError("Unsafe asset path")
            download(item,target)
    (source/"download-manifest.json").write_text(json.dumps(entry,indent=2))
    convert(folder,asset)
