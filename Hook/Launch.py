# Copyright mzoesch. All rights reserved.

import os
import json
import hmac
import hashlib
from fastapi import Header
from fastapi import FastAPI
from fastapi import Request
from fastapi import APIRouter
from fastapi import HTTPException
from fastapi import BackgroundTasks
from fastapi.responses import FileResponse
from fastapi.staticfiles import StaticFiles
from UpdateStaticMotorBins import update_static_motor
from Env import secret_x_hub_signature
from Env import vcs_local_remote_clone


router = APIRouter()


@router.get('/static/Motor/target/x86_64-unknown-linux-gnu/release/motor')
def get_motor_linux():
    real_path = f'{vcs_local_remote_clone}/Motor/target/x86_64-unknown-linux-gnu/release/motor'

    if os.path.exists(real_path):
        return FileResponse(
            real_path,
            media_type='application/octet-stream',
            filename='motor',
            headers={"Content-Disposition": "attachment; filename=motor"}
            )
    else:
        raise HTTPException(status_code=404)


@router.get('/static/Motor/target/x86_64-pc-windows-gnu/release/motor.exe')
def get_motor_win():
    real_path = f'{vcs_local_remote_clone}/Motor/target/x86_64-pc-windows-gnu/release/motor.exe'

    if os.path.exists(real_path):
        return FileResponse(
            real_path,
            media_type='application/octet-stream',
            filename='motor.exe',
            headers={"Content-Disposition": "attachment; filename=motor.exe"}
            )
    else:
        raise HTTPException(status_code=404)


@router.post('/post-push')
async def handle_post_req_sjafg_post_push(
    request: Request,
    background_tasks: BackgroundTasks,
    x_hub_signature_256: str = Header(None)
    ):

    if x_hub_signature_256 is None:
        raise HTTPException(status_code=400, detail='Missing X-Hub-Signature-256 header')

    body = await request.body()
    computed_signature = 'sha256=' + hmac.new(secret_x_hub_signature, body, hashlib.sha256).hexdigest()

    if not hmac.compare_digest(computed_signature, x_hub_signature_256):
        raise HTTPException(status_code=403, detail='Forbidden')

    json_payload = await request.json()

    if json_payload.get('action') != 'push':
        raise HTTPException(status_code=400, detail='Invalid action')

    background_tasks.add_task(update_static_motor)

    return \
        {
            'status': 'Webhook received and verified'
        }


app = FastAPI()
app.include_router(router)


# Preload the static motor binaries
update_static_motor()
