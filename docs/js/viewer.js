
// https://coolors.co/dcd6f7-a6b1e1-b4869f-985f6f-4e4c67
var enclosure_height = 148.786;
var enclosure_height_lower = 82.6451;

var branch = 'master';
var url_params = new URLSearchParams(location.search);
if (url_params.has('branch')) {
    branch = url_params.get('branch');
}

var model_base_url = 'https://s3.amazonaws.com/splitflap-artifacts/' + branch + '/3d/3d_colored_stl/';

var debug = false;

var error_container = document.getElementById("error-container");
var canvas = document.getElementById("viewer_canvas");

if (WEBGL.isWebGLAvailable()) {
  var scene = new THREE.Scene();
  var render_width = canvas.clientWidth;
  var render_height = canvas.clientHeight;

  scene.fog = new THREE.Fog(0x985F6F, 1200, 4000);
  var camera = new THREE.PerspectiveCamera( 35, render_width/render_height, 1, 4000 );
  camera.position.set(-400, 200, -400);

  var renderer = new THREE.WebGLRenderer({canvas: canvas});

  function addShadowedLight(x, y, z, color, intensity, top, right, bottom, left) {
      var directionalLight = new THREE.DirectionalLight( color, intensity );
      directionalLight.position.set( x, y, z );
      scene.add( directionalLight );
      directionalLight.castShadow = true;
      directionalLight.shadow.camera.left = left;
      directionalLight.shadow.camera.right = right;
      directionalLight.shadow.camera.top = top;
      directionalLight.shadow.camera.bottom = bottom;
      directionalLight.shadow.camera.near = 1200;
      directionalLight.shadow.camera.far = 1700;
      directionalLight.shadow.mapSize.width = 512;
      directionalLight.shadow.mapSize.height = 512;

      if (debug) {
          scene.add( new THREE.CameraHelper( directionalLight.shadow.camera ));
      }
  }

  // Ground
  var plane = new THREE.Mesh(
      new THREE.PlaneBufferGeometry(7000, 7000),
      new THREE.MeshPhongMaterial({color: 0x231A22, specular:0x101010})
  );
  plane.rotation.x = -Math.PI/2;
  plane.position.y = -0.1;
  plane.receiveShadow = true;
  scene.add(plane);

  // Lights
  scene.add( new THREE.HemisphereLight( 0x443333, 0x111122 ) );
  addShadowedLight( 300, 1000, -1000, 0xE6CBE9, 0.9, 150, 100, -50, -100);
  addShadowedLight( -300, 1500, -100, 0xF6F1C0, 0.6, 100, 80, -80, -80);

  // renderer
  renderer.setSize( render_width, render_height);
  renderer.setClearColor( scene.fog.color );
  renderer.setPixelRatio( window.devicePixelRatio );
  renderer.gammaInput = true;
  renderer.gammaOutput = true;
  renderer.shadowMap.enabled = true;

  if (debug) {
      var stats = new Stats();
      stats.showPanel( 0 );
      document.body.appendChild( stats.dom );
  }

  var render = function () {
      if (debug) {
          stats.begin();
      }

      controls.update();
      renderer.render(scene, camera);

      if (debug) {
          stats.end();
      }
      requestAnimationFrame(render);
  };

  // Camera Controls
  controls = new THREE.OrbitControls( camera, renderer.domElement );
  controls.enableZoom = true;
  controls.minDistance = 80;
  controls.maxDistance = 1300;
  controls.validUpdate = function(position, quaternion, target) {
      // Don't allow camera to go below ground
      return position.y > 0 && target.y > 0;
  };
  controls.autoRotateSpeed = 1.5;
  controls.addEventListener('start', function(){
    controls.autoRotate = false;
    $("#banner").fadeOut();
  });


  var cameraTarget = new THREE.Vector3(0, enclosure_height_lower, 0);
  controls.target = cameraTarget;
  controls.update();

  render();

  var expected_models = 0;
  var loaded_models = 0;
  var loader = new THREE.XHRLoader(THREE.DefaultLoadingManager);
  loader.load(model_base_url + 'manifest.json', function(text) {
      var json = JSON.parse(text);
      var keys = Object.keys(json)
      expected_models = keys.length;
      for (var i = 0; i < expected_models; i++) {
          var color = json[keys[i]];
          loadStl(model_base_url + keys[i], new THREE.Color( color[0], color[1], color[2] ).getHex());
      }
  });
  var loadStl = function(url, color) {
      var loader = new THREE.STLLoader();
      loader.load(url, function(geometry) {
          var material = new THREE.MeshPhongMaterial({
              color: color,
              specular: 0x111111,
              shininess: 10
          });
          var mesh = new THREE.Mesh(geometry, material);

          mesh.position.set(0, enclosure_height_lower, 0);
          mesh.rotation.set(-Math.PI/2, 0, 0);

          mesh.castShadow = true;
          mesh.receiveShadow = true;
          scene.add(mesh);

          loaded_models++;
          if (loaded_models === expected_models) {
              controls.autoRotate = true;
              $("#banner").fadeIn(1200);
          }
      });
  };

  if (canvas.classList.contains("embed")) {
      window.addEventListener('resize', function () {
          renderer.setSize(window.innerWidth, window.innerHeight);
          camera.aspect = window.innerWidth / window.innerHeight;
          camera.updateProjectionMatrix();
      });
  }
} else {
    var warning = WEBGL.getWebGLErrorMessage();
    error_container.appendChild(warning);
}
