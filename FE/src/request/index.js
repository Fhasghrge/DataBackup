const ws = new WebSocket("ws://127.0.0.1:9003");

ws.onopen = function (evt) {
  console.log("Connection open ...");
  ws.send(JSON.stringify({
    method: 'getPWD'
  }))
};
// ws.onmessage = function(evt) {
//   console.log( "Received Message: " + evt.data);
// };

ws.onclose = function (evt) {
  console.log("Connection closed.");
};



export function getCurrentPwd() {
  return new Promise((resolve, reject) => {
    ws.onmessage = function (evt) {
      const res = JSON.parse(evt.data)
      if (res.cwd) {
        resolve(res.cwd)
        console.log(res)
      }
    };
  })
}


export function getDirTrees(cwd) {
  return new Promise((resolve, reject) => {
    if (ws.readyState === 1) {
      ws.send(JSON.stringify({
        method: "getList",
        path: cwd
      }))
      ws.onmessage = function (evt) {
        const res = JSON.parse(evt.data)
        if (res.trees) {
          resolve(res.trees)
          console.log(res)
        }
      };
    }
  })
}

export function takeAction(action, source, target) {
  return new Promise((resolve, reject) => {
    if (ws.readyState === 1) {
      if (action === 'encrypt' | 'decrypt') {
        ws.send(JSON.stringify({
          method: action,
          source,
          target,
          key: '56513215'
        }))
      } else {
        ws.send(JSON.stringify({
          method: action,
          source,
          target
        }))
      }
    }
  })
}