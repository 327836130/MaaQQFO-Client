from pathlib import Path
import json, hashlib, argparse
p=argparse.ArgumentParser()
p.add_argument('root',type=Path)
p.add_argument('output',type=Path)
a=p.parse_args()
rows=[]
for f in sorted(a.root.rglob('*')):
    if not f.is_file() or any(x in f.parts for x in ('.git','node_modules','.venv','__pycache__')): continue
    if f.suffix.lower() not in ('.cpp','.h','.cs','.csproj','.md','.json','.png','.py','.ps1','.txt'): continue
    try:
        data=f.read_bytes()
        rows.append(dict(path=str(f.relative_to(a.root)),bytes=len(data),sha256=hashlib.sha256(data).hexdigest()))
    except OSError as e:
        rows.append(dict(path=str(f),error=str(e)))
a.output.parent.mkdir(parents=True,exist_ok=True)
a.output.write_text(json.dumps(rows,ensure_ascii=False,indent=2),encoding='utf-8')
print(f'{len(rows)} assets inventoried; content not copied')
