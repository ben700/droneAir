const path = require( 'path' )
const semver = require( 'semver' )
const { Storage } = require( '@google-cloud/storage' )
const { BigQuery } = require( '@google-cloud/bigquery' )


const ota = require('./firebaseOTAFunctions');
exports.getDownDroneAirLoadUrl = ota.getDownDroneAirLoadUrl; 
exports.insertDroneAirFirmwaresOnBigquery = ota.insertDroneAirFirmwaresOnBigquery; 
 