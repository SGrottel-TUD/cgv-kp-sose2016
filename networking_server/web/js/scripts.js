var game = game || {}; 
//
// Game logic
game.width = game.width || 4.0;
game.height = game.height || 3.0;
game.pos_eps = game.pos_eps || 0.0;
game.score = game.score || 0;
game.hands = game.hands || [];
game.stars = game.stars || [];

game.updateData = game.updateData || function(data) {
  game.width = data.width;
  game.height = data.height;
  game.pos_eps = data.pos_eps;
  game.score = data.score;
  game.stars = data.stars;
  game.render();
};
game.setHands = game.setHands || function(data) {
  game.hands = data;
  game.sendHands();
  game.render();
};
game.addHand = game.addHand || function(data) {
  game.hands[game.hands.length] = data;
  game.sendHands();
  game.render();
};
game.removeHand = game.removeHand || function(data) {
  game.sendHands();
  game.render();
};

//
// WebSocket
//
game.connection = game.connection || false;
game.setupConnection = game.setupConnection || function() {
  if (game.connection && game.connection.readyState === WebSocket.OPEN)
    return;
  console.log('[Connection] connecting...');
  game.connection = new WebSocket('wss://evilham.com:6602');
  game.connection.binaryType = 'arraybuffer';
  game.connection.onopen = game._onopen;
  game.connection.onclose = game._onclose;
  game.connection.onerror = game._onerror;
  game.connection.onmessage = game._onmessage;
}
game._onopen = function() {
  console.log('[Connection] established');
};
game._onclose = function() {
  console.log('[Connection] disconnected');
  game.setupConnection();
}
game._onerror = function(error) {
  console.log('[Connection] error', error);
};
game._onmessage = function(e) {
  var data = new DataView(e.data);
  var offset = 0;
  var out = {};
  while (offset < e.data.byteLength) {
    out = {};
    var endian = true;
    if (e.data.byteLength - offset < 24)
    {
      console.error('Invalid frame length');
      return {error: true};
    }
    out.width = data.getFloat32(offset, endian); offset += 4;
    out.height = data.getFloat32(offset, endian); offset += 4;
    out.pos_eps = data.getFloat32(offset, endian); offset += 4;
    out.gameMode = data.getInt16(offset, endian); offset += 4;
    out.score = data.getUint32(offset, endian); offset += 4;
    out.stars_count = data.getUint32(offset, endian); offset += 4;
    out.stars = [];
    var i = 0;
    var end_offset = offset + 28 * out.stars_count;
    while (offset < end_offset && i < out.stars_count) {
      out.stars[i] = {};
      out.stars[i].id = data.getUint32(offset, endian); offset +=4;
      out.stars[i].x = data.getFloat32(offset, endian); offset +=4;
      out.stars[i].y = data.getFloat32(offset, endian); offset +=4;
      out.stars[i].height = data.getFloat32(offset, endian); offset +=4;
      out.stars[i].dx = data.getFloat32(offset, endian); offset +=4;
      out.stars[i].dy = data.getFloat32(offset, endian); offset +=4;
      out.stars[i].inHand = (data.getUint32(offset, endian) === 1); offset +=4;

      out.stars[i].rel_x = out.stars[i].x / out.width;
      out.stars[i].rel_y = out.stars[i].y / out.height;
      ++i;
    }
  }
  game.updateData(out);
};
game.sendHands = game.sendHands || function() {
  var buf = new ArrayBuffer(4 + 12 * game.hands.length);
  var data = new DataView(buf);
  var offset = 0;
  var endian = true;
  data.setUint32(offset, game.hands.length, endian); offset += 4;
  for (var i = 0; i < game.hands.length; ++i)
  {
    data.setFloat32(offset, game.hands[i].x, endian); offset += 4;
    data.setFloat32(offset, game.hands[i].y, endian); offset += 4;
    data.setFloat32(offset, game.hands[i].h, endian); offset += 4;
  }
  console.log('[Connection] Sending', data);
  game.connection.send(buf);
};

//
// Graphic
//
game.render = game.render || function() {
  console.log('[Render]', game);
};
