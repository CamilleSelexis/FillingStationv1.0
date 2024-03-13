String Header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
const char homePageStr[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
 <head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
  <meta charset='utf-8'>
  <style>
    body {font-size:100%;}
    #main {display: table; margin: auto;  padding: 10px 10px 10px 10px; }
    #FillingStation{ border: 5px solid blue; border-radius: 15px; padding: 10px 10px 10px 10px; margin:10px 10px 10px 10px;}
    h2 {text-align:left; margin: 10px 0px 10px 0px;}
    h3 {text-align:center; margin: 5px 0px 5px 0px; font-size: 160%}
    p {margin: 5px 0px 10px 0px; font-size: 120%;}
    .button {padding: 5px 5px 5px 5px; font-size:110%}
    .number {height:35px; width:100px; font-size:110%}
    .status {border: 5px solid blue; border-radius: 15px; padding: 5px 5px 5px 5px; float: right; width: 50px;}
    .manualControl{border: 5px solid blue; border-radius: 15px; padding: 10px 10px 10px 10px; float: left; width: 45%; height:180px;margin:10px 10px 10px 10px;}
    .manualControl2{border: 5px solid blue; border-radius: 15px; padding: 10px 10px 10px 10px; float: left; width: 45%; height:190px;margin:10px 10px 10px 10px;}
  </style>
 
  <script>
    function updateTime()
    {  
       var d = new Date();
       var t = "";
       t = d.toLocaleTimeString();
       document.getElementById('P_time').innerHTML = t;
    }
     var logCounter = 0;
    var ajaxRequest = null;
    if (window.XMLHttpRequest)  { ajaxRequest =new XMLHttpRequest(); }
    else                        { ajaxRequest =new ActiveXObject("Microsoft.XMLHTTP"); }
 
    function updateStartPage()
    {
      if(!ajaxRequest){ alert('AJAX is not supported.'); return; }
 
      ajaxRequest.open('GET','updateStartPage',true);
      ajaxRequest.onreadystatechange = function()
      {
        if(ajaxRequest.readyState == 4 && ajaxRequest.status==200)
        {
          var ajaxResult = ajaxRequest.responseText;
          var tmpArray = ajaxResult.split("|");
//Results are split as follow: Time_Alive;PumpState;PumpVolLeft;ValvePort;CurrentWeight_1;ScaleState1
            //CurrentWeight_2;ScaleState2
            if(tmpArray.length>12){

              var TimeAliveDay = Math.floor(tmpArray[0]/86400);
              tmpArray[0] = tmpArray[0]-86400*TimeAliveDay;
              var TimeAliveHour = Math.floor(tmpArray[0]/3600);
              tmpArray[0] = tmpArray[0]-3600*TimeAliveHour;
              var TimeAliveMin = Math.floor(tmpArray[0]/60);
              tmpArray[0] = tmpArray[0]-60*TimeAliveMin;
              var TimeAliveSec = Math.floor(tmpArray[0]);

              document.getElementById('Time_Alive_d').innerHTML = TimeAliveDay;
              document.getElementById('Time_Alive_h').innerHTML = TimeAliveHour;
              document.getElementById('Time_Alive_m').innerHTML = TimeAliveMin;
              document.getElementById('Time_Alive_s').innerHTML = TimeAliveSec;
              
              if(tmpArray[1] == 0){
                document.getElementById('Valve').style.border = '5px solid red';
                document.getElementById('ValveStatus').style.border = '5px solid red';
                document.getElementById('valveState').innerHTML = 'Not Init';
              }
              if(tmpArray[1] == 1){
                document.getElementById('Valve').style.border = '5px solid green';
                document.getElementById('ValveStatus').style.border = '5px solid green';
                document.getElementById('valveState').innerHTML = 'Idle';
              }
              if(tmpArray[1] == 2){
                document.getElementById('Valve').style.border = '5px solid yellow';
                document.getElementById('ValveStatus').style.border = '5px solid yellow';
                document.getElementById('valveState').innerHTML = 'Running';
              }
              
              document.getElementById('valvePort').innerHTML = tmpArray[2];

              if(tmpArray[3] == 0){
                document.getElementById('Pump').style.border = '5px solid red';
                document.getElementById('PumpStatus').style.border = '5px solid red';
                document.getElementById('pumpState').innerHTML = 'Not Init';
              }
              if(tmpArray[3] == 1){
                document.getElementById('Pump').style.border = '5px solid green';
                document.getElementById('PumpStatus').style.border = '5px solid green';
                document.getElementById('pumpState').innerHTML = 'Idle';
              }
              if(tmpArray[3] == 2){
                document.getElementById('Pump').style.border = '5px solid yellow';
                document.getElementById('PumpStatus').style.border = '5px solid yellow';
                document.getElementById('pumpState').innerHTML = 'Running';
              }

              document.getElementById('currentFlow').innerHTML = tmpArray[4];

              if(tmpArray[5] == 0){
                document.getElementById('Scale1').style.border = '5px solid red';
                document.getElementById('ScaleState1').innerHTML = 'Not Init';
                document.getElementById('ScaleStatus1').style.border = '5px solid red';
              }
              if(tmpArray[5] == 1){
                document.getElementById('Scale1').style.border = '5px solid green';
                document.getElementById('ScaleState1').innerHTML = 'Active';
                document.getElementById('ScaleStatus1').style.border = '5px solid green';
              }
              if(tmpArray[5] == 2){
                document.getElementById('Scale1').style.border = '5px solid yellow';
                document.getElementById('ScaleState1').innerHTML = 'Backup';
                document.getElementById('ScaleStatus1').style.border = '5px solid yellow';
              }
              document.getElementById('Current_Weight1').innerHTML = tmpArray[6];
              document.getElementById('Current_Calibration1').innerHTML = tmpArray[7];

              if(tmpArray[8] == 0){
                document.getElementById('Scale2').style.border = '5px solid red';
                document.getElementById('ScaleState2').innerHTML = 'Not Init';
                document.getElementById('ScaleStatus2').style.border = '5px solid red';
              }
              if(tmpArray[8] == 1){
                document.getElementById('Scale2').style.border = '5px solid green';
                document.getElementById('ScaleState2').innerHTML = 'Active';
                document.getElementById('ScaleStatus2').style.border = '5px solid green';
              }
              if(tmpArray[8] == 2){
                document.getElementById('Scale2').style.border = '5px solid yellow';
                document.getElementById('ScaleState2').innerHTML = 'Backup';
                document.getElementById('ScaleStatus2').style.border = '5px solid yellow';
              }
              document.getElementById('Current_Weight2').innerHTML = tmpArray[9];
              document.getElementById('Current_Calibration2').innerHTML = tmpArray[10];
              
              if(tmpArray[11] == 0){
                document.getElementById('FillingStation').style.border = '5px solid red';
                document.getElementById('fillStationState').innerHTML = 'Not Init';
                document.getElementById('FillStationStatus').style.border = '5px solid red';
              }
              if(tmpArray[11] == 1){
                document.getElementById('FillingStation').style.border = '5px solid green';
                document.getElementById('fillStationState').innerHTML = 'Init';
                document.getElementById('FillStationStatus').style.border = '5px solid green';
              }
              if(tmpArray[11] == 2){
                document.getElementById('FillingStation').style.border = '5px solid yellow';
                document.getElementById('fillStationState').innerHTML = 'Busy';
                document.getElementById('FillStationStatus').style.border = '5px solid yellow';
              }
              if(tmpArray[12] == 0){
                document.getElementById('RinseStation').style.border = '5px solid red';
                document.getElementById('rinseState').innerHTML = 'Not Init';
                document.getElementById('RinseStatus').style.border = '5px solid red';
              }
              if(tmpArray[12] == 1){
                document.getElementById('RinseStation').style.border = '5px solid green';
                document.getElementById('rinseState').innerHTML = 'Init';
                document.getElementById('RinseStatus').style.border = '5px solid green';
              }
              if(tmpArray[12] == 2){
                document.getElementById('RinseStation').style.border = '5px solid yellow';
                document.getElementById('rinseState').innerHTML = 'Busy';
                document.getElementById('RinseStatus').style.border = '5px solid yellow';
              }
              document.getElementById('currentMultiplier').innerHTML = tmpArray[13];
              document.getElementById('lastDispense').innerHTML = tmpArray[14];
            }
          }
        }
      ajaxRequest.send();
      
    }
   
    function ajaxCommand(ajaxURL)
    {           
      if(!ajaxRequest){ alert('AJAX is not supported.'); return; }
      switch(ajaxURL){
        case '/goToPort':
            ajaxURL = ajaxURL.concat('/',document.getElementById('PortNumber').value);
            break;
        case '/setFlow':
            ajaxURL = ajaxURL.concat('/',document.getElementById('flowRate').value);
            break;
        case '/dispenseVol':
            ajaxURL = ajaxURL.concat('/',document.getElementById('DispenseVol').value);
            break;
        case '/setCalibration1':
            ajaxURL = ajaxURL.concat('/',document.getElementById('CalibrationFactor1').value);
            break;
        case '/setCalibration2':
            ajaxURL = ajaxURL.concat('/',document.getElementById('CalibrationFactor2').value);
            break;
        case '/dispenseMediaVol':
            ajaxURL = ajaxURL.concat('/',document.getElementById('MediaVolume').value);
            break;
        case '/dispenseAir':
          ajaxURL = ajaxURL.concat('/',document.getElementById('airTime').value);
            break;
        case '/dispenseClean1':
          ajaxURL = ajaxURL.concat('/',document.getElementById('dispenseClean1').value);
            break;
        case '/dispenseClean2':
          ajaxURL = ajaxURL.concat('/',document.getElementById('dispenseClean2').value);
            break;
        case '/enableUVC':
          ajaxURL = ajaxURL.concat('/',document.getElementById('enableUVC').value);
            break;
        case '/setVolumeMultiplier':
          ajaxURL = ajaxURL.concat('/',document.getElementById('volumeMultiplier').value);
            break;
      }
      ajaxRequest.open('GET',ajaxURL,true);
      ajaxRequest.onreadystatechange = function()
      {
        if(ajaxRequest.readyState == 4 && ajaxRequest.status==200)
        {
          var ajaxResult = ajaxRequest.responseText;
        }
      }
      ajaxRequest.send();
      alert(ajaxURL);
    }
    var myVar1 = setInterval(updateStartPage, 5000);  
    var myVar2 = setInterval(updateTime, 1000);  
  </script>

    <title>Filling Station Controller</title>
 </head>
 
 <body>
   <div id='main'>
      <div id='FillingStation'>
        <p style="font-size:160%;"> <b>Filling Station Controller</b> <span id='P_time' style="float:right">Time</span></p><div id="FillStationStatus" class="status"><i id="fillStationState">Status</i></div>
        
        <p> Time Alive : <b id ='Time_Alive_d'>Day</b> / <b id ='Time_Alive_h'>Hour</b> : <b id ='Time_Alive_m'>Min</b> : <b id ='Time_Alive_s'>Sec</b></p>
        <input type="button" class="button" onclick="ajaxCommand('/initializeFillStation')" value="Initialize Filling Station">
        <input type="button" class="button" onclick="ajaxCommand('/primeLinesFillStation')" value="Prime Filling Station"><br/>
        <input type="button" class="button" onclick="ajaxCommand('/dispenseMediaVol')" value="Dispense Media">
        <input type="number" class="number" id="MediaVolume" name="MediaVolume" min="0" max="100" step="0.1" value="0.0"> mL 
        Last Dispense: <span id="lastDispense"> lastdispense </span>g<br/>
        <input type="button" class="button" onclick="ajaxCommand('/cleanFillStation')" value="Clean Filling Station">
        <input type="button" class="button" onclick="ajaxCommand('/flushFillStation')" value="Flush with Air"> <br/>
        <input type="button" class="button" onclick="ajaxCommand('/rinseTip')" value="Rinse distribution tip">
        <input type="button" class = "button" style="float:right" onclick="ajaxCommand('/resetController')" value="Reset Controller">
      </div>
     <div id='Valve' class="manualControl">
       <h3> Valve </h3><div id="ValveStatus" class="status"><i id="valveState">Status</i></div>
       <p style="text-align:left">
       <input type="button" class="button" onclick="ajaxCommand('/initializeValve')" value="Initialize"><br/>
       <input type="button" class="button" onclick="ajaxCommand('/goToPort')" value="GoToPort">
       <input type="number" class="number" id="PortNumber" name="PortNumber" min="1" max="8" value="1"><span style="float:right;">Current Port : <span id="valvePort"> ValvePort </span></span><br/>
       </p>
     </div>
     <div id='Pump'class="manualControl">
       <h3> Pump </h3><div id="PumpStatus" class="status"><i id="pumpState">Status</i></div>
       <p style="text-align:left">
       <input type="button" class="button" onclick="ajaxCommand('/initializePump')" value="Initialize"><br/>
       <input type="button" class="button" onclick="ajaxCommand('/setFlow')" value="Set Flow Rate">
       <input type="number" class="number" id="flowRate" name="flowRate" min="10" max="2000" value="1000"> uL/s <span style="float:right"> Actual: <span id="currentFlow"> flowRate </span> uL/s </span><br/>
       <input type="button" class="button" onclick="ajaxCommand('/dispenseVol')" value="Dispense">
       <input type="number" class="number" id="DispenseVol" name="DispenseVol" min="0" max="100" value="0.0" step="0.1"> mL <br/>
       <input type="button" class="button" onclick="ajaxCommand('/setVolumeMultiplier')" value="Set Multiplier">
       <input type="number" class="number" id="volumeMultiplier" name="volumeMultiplier" min="0" max="2" value="1.0" step="0.01">
       <span style="float:right"> Current multiplier: <span id="currentMultiplier"> 1 </span> </span><br/>
     </div>
     <div id='Scale1'class="manualControl">
       <h3> Scale 1 </h3><div id="ScaleStatus1" class="status"><i id="ScaleState1">Status</i></div>
       <input type="button" class="button" onclick="ajaxCommand('/selectScale1')" value="Select Scale"style="float:right">
       <p style="text-align:left">
       <input type="button" class="button" onclick="ajaxCommand('/initializeScale1')" value="Initialize"><br/>
       <input type="button" class="button" onclick="ajaxCommand('/tareScale1')" value="Tare"><span style="float:right"> Current Weight: <span id="Current_Weight1"> Weight1 </span>g </span> <br/>
       <input type="button" class="button" onclick="ajaxCommand('/setCalibration1')" value="Set Calibration Factor">
       <input type="number" class="number" id="CalibrationFactor1" name="CalibrationFactor1" min="-10000" max="10000" value="0">
       <span style="float:right"> Current calibration: <span id="Current_Calibration1"> -1000 </span> </span><br/>
     </div>
     <div id='Scale2'class="manualControl">
       <h3> Scale 2 </h3><div id="ScaleStatus2" class="status"><i id="ScaleState2">Status</i></div>
       <input type="button" class="button" onclick="ajaxCommand('/selectScale2')" value="Select Scale"style="float:right">
       <p style="text-align:left">
       <input type="button" class="button" onclick="ajaxCommand('/initializeScale2')" value="Initialize"><br/>
       <input type="button" class="button" onclick="ajaxCommand('/tareScale2')" value="Tare"><span style="float:right"> Current Weight: <span id="Current_Weight2"> Weight2 </span>g </span> <br/>
       <input type="button" class="button" onclick="ajaxCommand('/setCalibration2')" value="Set Calibration Factor">
       <input type="number" class="number" id="CalibrationFactor2" name="CalibrationFactor2" min="-10000" max="10000" value="0">
       <span style="float:right"> Current calibration: <span id="Current_Calibration2"> -1000 </span> </span><br/>
     </div>
     <div id='RinseStation' class="manualControl2">
       <h3> Rinse Station </h3><div id="RinseStatus" class="status"><i id="rinseState">Status</i></div>
       <p style="text-align:left">
       <input type="button" class="button" onclick="ajaxCommand('/dispenseAir')" value="DispenseAir">
       <input type="number" class="number" id="airTime" name="airTime" min="0" max="60" value="5"> s<br/>
       <input type="button" class="button" onclick="ajaxCommand('/dispenseClean1')" value="Dispense Clean Solution 1">
       <input type="number" class="number" id="dispenseClean1" name="dispenseClean1" min="0" max="100" value="0.0" step="0.1"> mL <br/>
       <input type="button" class="button" onclick="ajaxCommand('/dispenseClean2')" value="Dispense Clean Solution 2">
       <input type="number" class="number" id="dispenseClean2" name="dispenseClean2" min="0" max="100" value="0.0" step="0.1"> mL <br/>
       <input type="button" class="button" onclick="ajaxCommand('/enableUVC')" value="Enable UVC Led">
       <input type="number" class="number" id="enableUVC" name="enableUVC" min="0" max="200" value="0.0" step="0.1"> s <br/>
     </div>
   </div>
 </body>
</html>

)=====";
