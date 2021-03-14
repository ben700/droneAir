

 ~/google-cloud-sdk/bin/gcloud config set project drone-302200
 ~/google-cloud-sdk/bin/gcloud functions deploy insertDroneAirFirmwaresOnBigquery --region=europe-west2 --runtime nodejs10 --trigger-resource droneair-firmwares --trigger-event google.storage.object.finalize
 ~/google-cloud-sdk/bin/gcloud functions deploy getDownDroneAirLoadUrl --region=europe-west2 --runtime nodejs10 --trigger-http
