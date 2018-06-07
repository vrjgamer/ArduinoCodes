var admin = require('firebase-admin')
var serviceAccount = require('serviceAccountKey.json')

admin.initializeApp({
  credential: admin.credential.cert(serviceAccount),
  databaseURL: 'https://smartplugiot-89f88.firebaseio.com'
})
