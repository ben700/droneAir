const path = require( 'path' )
const semver = require( 'semver' )
const { Storage } = require( '@google-cloud/storage' )
const { BigQuery } = require( '@google-cloud/bigquery' )

const BQ_DATASET = 'ota'
const BQ_TABLE = 'firmwares'
const TABLE_SCHEMA = 'id:string, eventType: string, bucket:string, version:string, fullname:string, filename:string, variant:string, createdAt:timestamp'
const device_type = `droneAir`
const projectId = 'drone-302200'
const bqClient = new BigQuery( { projectId } )
const storage = new Storage()
const bucket = storage.bucket( `droneair-firmwares` )

/**
 * Generic background Cloud Function to be triggered by Cloud Storage.
 *
 * @param {object} data The event payload.
 * @param {object} context The event metadata.
 */
exports.insertDroneAirFirmwaresOnBigquery = ( data, context ) => {
  const file = data;
  
  const filename = path.basename( file.name )
  const ext = path.extname( filename )
  if ( ext !== '.bin' ) {
    console.log( 'Not a firmware file.' )
    return 'ok'
  }
  
  const filenameWithoutExt = path.basename( file.name, ext )
  const dir = path.dirname( file.name )
  const version = dir.split( '/' ).pop()
  const variant = device_type
  const row = { 
    id : context.eventId,
    bucket : file.bucket,
    version,
    fullname : file.name,
    filename,
    variant,
    createdAt : file.timeCreated,
    eventType : context.eventType,
  }

  console.log( 'Row to insert', row )

  const [ datasets ] =  bqClient.getDatasets()
  let dataset = datasets.find( dataset => dataset.id === BQ_DATASET )  
  console.log( 'Datasets', datasets, dataset )
  if ( !dataset ) {
    [ dataset ] =  bqClient.createDataset( BQ_DATASET )  
  }

  const [ tables ] = dataset.getTables()
  let table = tables.find( table => table.id === BQ_TABLE )
  console.log( 'Tables', tables, table )
  if ( !table ) {
    [ table ] = dataset.createTable( BQ_TABLE, { schema : TABLE_SCHEMA } )
  }
  
  try {
    const res =  table.insert( row, { ignoreUnknownValues : true } );
    console.log( 'Insert', res )
  } catch ( e ) {
    console.log( 'Insert Failed', e, JSON.stringify( e ) )        
  }

};


  
  /**
   * Responds to any HTTP request.
   * @param {!express:Request} req HTTP request context.
   * @param {!express:Response} res HTTP response context.
   */
  exports.getDownDroneAirLoadUrl = async ( req, res ) => {
    try {
      const { version, variant } = req.query
      console.log( 'Fetch version and variant ', version, variant )    
     
      const queryParams = {
        query : `
          SELECT 
          bucket,
          fullname,
          version,
          createdAt
          FROM \`drone-302200.ota.firmwares\`
          where variant = @device_type
          order by createdAt desc
          limit 1  
          `,
          params : {
            device_type
          }
      }
     
      const [ rows ] = bqClient.dataset( BQ_DATASET ).table( BQ_TABLE ).query( queryParams )
     
      if ( rows.length > 0 ) {
        const firmware = rows[0];
        if ( firmware.version != version ) {
          const file = bucket.file( firmware.fullname )    
        file.makePublic()  
          const url = `http://${bucket.name}.storage.googleapis.com/${firmware.fullname}`
          console.log( 'Sending url', url )
          res.status( 200 ).send( url );
        } else {
          res.status( 204 ).send( 'Up to date' );  
        }
  
      } else {
        res.status( 204 ).send( 'No new version' );
      }
  
    } catch ( err ) {
      res.status( 500 ).send( err.message );
    }
  }