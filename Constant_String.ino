String Header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
//const char headerOK[] PROGMEM = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
const char homePageStr[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
 <head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
  <meta charset='utf-8'>
  <style>
    body {font-size:100%;}
    #main {display: table; margin: auto;  padding: 10px 10px 10px 10px; }
    #Valve { border: 5px solid blue; border-radius: 15px; padding: 10px 10px 10px 10px; float: left; width: 45%; height:180px; margin:10px 10px 10px 10px;}
    #ValveStatus { border: 5px solid blue; border-radius: 15px; padding: 5px 5px 5px 5px; float: right; width: 50px;}
    #Pump { border: 5px solid blue; border-radius: 15px; padding: 10px 10px 10px 10px; float: right; width: 45%; height:180px; margin:10px 10px 10px 10px;}
    #PumpStatus { border: 5px solid blue; border-radius: 15px; padding: 5px 5px 5px 5px; float: right; width: 50px;}
    #Scale1 { border: 5px solid blue; border-radius: 15px; padding: 10px 10px 10px 10px; float: left; width: 45%; margin:10px 10px 10px 10px;}
    #ScaleStatus1 { border: 5px solid blue; border-radius: 15px; padding: 5px 5px 5px 5px; float: right; width: 50px; }
    #Scale2 { border: 5px solid blue; border-radius: 15px; padding: 10px 10px 10px 10px; float: right; width: 45%; margin:10px 10px 10px 10px;}
    #ScaleStatus2 { border: 5px solid blue; border-radius: 15px; padding: 5px 5px 5px 5px; float: right; width: 50px; }
    #Components { border: 5px solid blue; border-radius: 15px; padding: 10px 0px 10px 0px;}
    #content { border: 5px solid blue; border-radius: 15px; padding: 10px 0px 10px 0px;}
    #commands { border: 3px solid yellow; border-radius: 9px; padding: 5px 5px 5px 5px; margin:10px 10px 10px 10px; float:right;}
    h2 {text-align:center; margin: 10px 0px 10px 0px;}
    h3 {text-align:center; margin: 5px 0px 5px 0px; font-size: 160%}
    p { text-align:center; margin: 5px 0px 10px 0px; font-size: 120%;}
    td { text-align:center; font-size: 120%;}
    #time_P { margin: 10px 0px 15px 0px;}
    button {padding: 10px 10px 10px 10px;}
  </style>
 
  <script>
    function updateTime()
    {  
       var d = new Date();
       var t = "";
       t = d.toLocaleTimeString();
       document.getElementById('P_time').innerHTML = t;
    }
   
    var ajaxRequest = null;
    if (window.XMLHttpRequest)  { ajaxRequest =new XMLHttpRequest(); }
    else                        { ajaxRequest =new ActiveXObject("Microsoft.XMLHTTP"); }
   
    function updateStartPage()
    {
      ajaxLoad('updateStartPage');
    }
 
    function ajaxLoad(ajaxURL)
    {
     
      if(!ajaxRequest){ alert('AJAX is not supported.'); return; }
 
      ajaxRequest.open('GET',ajaxURL,true);
      ajaxRequest.onreadystatechange = function()
      {
        if(ajaxRequest.readyState == 4 && ajaxRequest.status==200)
        {
          var ajaxResult = ajaxRequest.responseText;
          var tmpArray = ajaxResult.split("|");
//Results are split as follow: Time_Alive;PumpState;PumpVolLeft;ValvePort;CurrentWeight_1;ScaleState1
            //CurrentWeight_2;ScaleState2
            if(tmpArray.length>6){

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
                document.getElementById('ValveStatus').style.border = '5px solid red';
                document.getElementById('valveState').innerHTML = 'Not Init';
              }
              if(tmpArray[1] == 1){
                document.getElementById('ValveStatus').style.border = '5px solid green';
                document.getElementById('valveState').innerHTML = 'Idle';
              }
              if(tmpArray[1] == 2){
                document.getElementById('ValveStatus').style.border = '5px solid yellow';
                document.getElementById('valveState').innerHTML = 'Running';
              }
              document.getElementById('valvePort').innerHTML = tmpArray[2];
              if(tmpArray[3] == 0){
                document.getElementById('PumpStatus').style.border = '5px solid red';
                document.getElementById('pumpState').innerHTML = 'Not Init';
              }
              if(tmpArray[3] == 1){
                document.getElementById('PumpStatus').style.border = '5px solid green';
                document.getElementById('pumpState').innerHTML = 'Idle';
              }
              if(tmpArray[3] == 2){
                document.getElementById('PumpStatus').style.border = '5px solid yellow';
                document.getElementById('pumpState').innerHTML = 'Running';
              }
              //document.getElementById('flowRate').innerHTML = tmpArray[4];
            
              document.getElementById('ScaleState1').innerHTML = tmpArray[5]?'Active':'Backup';
              document.getElementById('ScaleStatus1').style.border = tmpArray[5]?'5px solid yellow':'5px solid green';
              document.getElementById('Current_Weight1').innerHTML = tmpArray[6];
              document.getElementById('Current_Calibration1').innerHTML = tmpArray[7];


              document.getElementById('ScaleState2').innerHTML = tmpArray[8]?'Active':'Backup';
              document.getElementById('ScaleStatus2').style.border = tmpArray[8]?'5px solid yellow':'5px solid green';
              document.getElementById('Current_Weight2').innerHTML = tmpArray[9];
              document.getElementById('Current_Calibration2').innerHTML = tmpArray[10];

            }
          }
        }
      ajaxRequest.send();
    }
   
   
    function ajaxCommand(ajaxURL)
    {
      alert(ajaxURL);
      document.getElementById('ValveStatus').style.border = '5px solid yellow';
    }
    var myVar1 = setInterval(updateStartPage, 5000);  
    var myVar2 = setInterval(updateTime, 1000);  
 
  </script>

    <title>Filling Station Controller</title>
 </head>
 
 <body>
   <div id='main'>
     <h2>Filling Station Controller</h2>
      <p id='P_time'>Time</p>
      <p> Time Alive : <b id ='Time_Alive_d'>Day</b> / <b id ='Time_Alive_h'>Hour</b> : <b id ='Time_Alive_m'>Min</b> : <b id ='Time_Alive_s'>Sec</b></p>
     <div id='Valve'>
       <h3> Valve </h3><div id="ValveStatus"><i id="valveState">Status</i></div>
       <p style="text-align:left">
       <input type="button" style="padding: 5px 5px 5px 5px; font-size:110%" onclick="ajaxCommand('/initializeValve')" value="Initialize" size="25"><br/>
       <input type="button" style="padding: 5px 5px 5px 5px; font-size:110%" onclick="ajaxCommand('/goToPort')" value="GoToPort">
       <input type="number" style="padding: 5px 5px 5px 5px; font-size:110%" id="PortNumber" name="PortNumber" min="1" max="8" value="1"><span style="float:right;">Current Port : <span id="valvePort"> ValvePort </span></span><br/>
       </p>
     </div>
     <div id='Pump'>
       <h3> Pump </h3><div id="PumpStatus"><i id="pumpState">Status</i></div>
       <p style="text-align:left">
       <input type="button" style="padding: 5px 5px 5px 5px; font-size:110%" onclick="ajaxCommand('/initializePump')" value="Initialize" size="25"><br/>
       <input type="button" style="padding: 5px 5px 5px 5px; font-size:110%" onclick="ajaxCommand('/setFlow')" value="Set Flow Rate">
       <input type="number" style="height:35px; width:100px; font-size:110%" id="flowRate" name="flowRate" min="10" max="2000" value="1000"> uL/s <span style="float:right"> Actual: <span id="flowRate"> flowRate </span> uL/s </span><br/>
       <input type="button" style="padding: 5px 5px 5px 5px; font-size:110%" onclick="ajaxCommand('/DispenseVol')" value="Dispense">
       <input type="number" style="height:35px; width:100px; font-size:110%" id="DispenseVol" name="DispenseVol" min="0" max="100" value="1" step="0.1"> mL
     </div>
     <div id='Scale1'>
       <h3> Scale 1 </h3><div id="ScaleStatus1"><i id="scaleState1">Status</i></div>
       <p style="text-align:left">
       <input type="button" style="padding: 5px 5px 5px 5px; font-size:110%" onclick="ajaxCommand('/initializeScale1')" value="Initialize" size="25"><br/>
       <input type="button" style="padding: 5px 5px 5px 5px; font-size:110%" onclick="ajaxCommand('/tareScale1')" value="Tare"><span style="float:right"> Current Weight: <span id="Current_Weight1"> Weight1 </span>g </span> <br/>
       <input type="button" style="padding: 5px 5px 5px 5px; font-size:110%" onclick="ajaxCommand('/SetCalibration1')" value="Set Calibration Factor">
       <span style="float:right"> Current calibration: <span id="Current_Calibration1"> -1000 </span> </span><br/>
     </div>
     <div id='Scale2'>
       <h3> Scale 2 </h3><div id="ScaleStatus2"><i id="scaleState1">Status</i></div>
       <p style="text-align:left">
       <input type="button" style="padding: 5px 5px 5px 5px; font-size:110%" onclick="ajaxCommand('/initializeScale2')" value="Initialize" size="25"><br/>
       <input type="button" style="padding: 5px 5px 5px 5px; font-size:110%" onclick="ajaxCommand('/tareScale2')" value="Tare"><span style="float:right"> Current Weight: <span id="Current_Weight2"> Weight2 </span>g </span> <br/>
       <input type="button" style="padding: 5px 5px 5px 5px; font-size:110%" onclick="ajaxCommand('/SetCalibration2')" value="Set Calibration Factor">
       <span style="float:right"> Current calibration: <span id="Current_Calibration2"> -1000 </span> </span><br/>
     </div>
     <div id='commands'>
     <input type="button" onclick="ajaxCommand('/resetController')" value="Reset Controller">
     </div>
   </div>
 </body>
</html>
)=====";
