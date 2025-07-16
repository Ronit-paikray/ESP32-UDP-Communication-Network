const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE HTML>
<html>
  <head>
    <title>Communication Between Multiple ESP32s via UDP (WiFi UDP)</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      html {font-family: Arial; display: inline-block; text-align: center;}
      p {font-size: 1.2rem;}
      h4 {font-size: 0.8rem;}
      body {margin: 0;}
      .topnav {overflow: hidden; background-color: #00878F; color: white; font-size: 0.65rem; height: 50px; display: flex; justify-content: center; align-items: center;}
      .content {padding: 5px;}
      .card {background-color: white; box-shadow: 0px 0px 10px 1px rgba(140,140,140,.5); border: 1px solid #00878f; border-radius: 15px;}
      .card.header {background-color: #00878f; color: white; border-bottom-right-radius: 0px; border-bottom-left-radius: 0px; border-top-right-radius: 12px; border-top-left-radius: 12px;}
      .cards {max-width: 700px; margin: 0 auto; display: grid; grid-gap: 1.5rem; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));}
      
      .myButton {
        display: inline-block;
        padding: 3px 17px;
        font-size: 13px;
        cursor: pointer;
        text-align: center;
        text-decoration: none;
        outline: none;
        color: #fff;
        border: none;
        border-radius: 8px;
        box-shadow: 0 2px #999;
      }
      .myButton:active {box-shadow: 0 1px #666; transform: translateY(2px);}
      
      .LS {font-size: 13px; color: #4d5861;}
    </style>
  </head>
  
  <body>
    <div class="topnav">
      <h1>Controlling ESP32 Devices via UDP & Web Server</h1>
    </div>
    
    <br>
    
    <!-- __ DISPLAYS MONITORING AND CONTROLLING ____________________________________________________________________________________________ -->
    <div class="content">
      <div class="cards"> 
        <!-- == ESP32 A or Slave A ================================================================================= -->
        <div class="card">
          <div class="card header">
            <h2 style="font-size: 1.0rem;">ESP32 A</h2>
          </div>
          
          <br>
          
          <button type="button" class="myButton" style="background-color: #1976D2;" onmouseover="this.style.backgroundColor ='#104d89';" onmouseout="this.style.backgroundColor ='#1976D2';" onclick="btn_Click('ESP32A','LED_1_ON')">LED 1 ON</button>
          <button type="button" class="myButton" style="background-color: #F5004F;" onmouseover="this.style.backgroundColor ='#780127';" onmouseout="this.style.backgroundColor ='#F5004F';" onclick="btn_Click('ESP32A','LED_1_OFF')">LED 1 OFF</button>
          
          <br><br>
          
          <button type="button" class="myButton" style="background-color: #1976D2;" onmouseover="this.style.backgroundColor ='#104d89';" onmouseout="this.style.backgroundColor ='#1976D2';" onclick="btn_Click('ESP32A','LED_2_ON')">LED 2 ON</button>
          <button type="button" class="myButton" style="background-color: #F5004F;" onmouseover="this.style.backgroundColor ='#780127';" onmouseout="this.style.backgroundColor ='#F5004F';" onclick="btn_Click('ESP32A','LED_2_OFF')">LED 2 OFF</button>
          
          <br>
          
          <P class="LS"><b>Status of LEDs</b></P>
          <p class="LS">LED 1 : <span id="sta_LED_1_ESP32_A">Wait...</span></p>
          <p class="LS">LED 2 : <span id="sta_LED_2_ESP32_A">Wait...</span></p>
        </div>
        <!-- ======================================================================================================= -->
        
        
        
        <!-- == ESP32 B or Slave B ================================================================================= -->
        <div class="card">
          <div class="card header">
            <h2 style="font-size: 1.0rem;">ESP32 B</h2>
          </div>
          
          <br>
          
          <button type="button" class="myButton" style="background-color: #1976D2;" onmouseover="this.style.backgroundColor ='#104d89';" onmouseout="this.style.backgroundColor ='#1976D2';" onclick="btn_Click('ESP32B','LED_1_ON')">LED 1 ON</button>
          <button type="button" class="myButton" style="background-color: #F5004F;" onmouseover="this.style.backgroundColor ='#780127';" onmouseout="this.style.backgroundColor ='#F5004F';" onclick="btn_Click('ESP32B','LED_1_OFF')">LED 1 OFF</button>
          
          <br><br>
          
          <button type="button" class="myButton" style="background-color: #1976D2;" onmouseover="this.style.backgroundColor ='#104d89';" onmouseout="this.style.backgroundColor ='#1976D2';" onclick="btn_Click('ESP32B','LED_2_ON')">LED 2 ON</button>
          <button type="button" class="myButton" style="background-color: #F5004F;" onmouseover="this.style.backgroundColor ='#780127';" onmouseout="this.style.backgroundColor ='#F5004F';" onclick="btn_Click('ESP32B','LED_2_OFF')">LED 2 OFF</button>
          
          <br>
          
          <P class="LS"><b>Status of LEDs</b></P>
          <p class="LS">LED 1 : <span id="sta_LED_1_ESP32_B">Wait...</span></p>
          <p class="LS">LED 2 : <span id="sta_LED_2_ESP32_B">Wait...</span></p>
        </div>  
        <!-- ======================================================================================================= -->
        
      </div>
    </div>
    <!-- ___________________________________________________________________________________________________________________________________ -->
    
    <script>
      // Command to the Master (Server) to send commands to all slaves so that all slaves send LEDs status.
      Send("set","id=ALL&cmd=GSAL");
      
      // Two seconds after the command is sent,
      // the command to get the status of the LEDs that the master has received from the Slaves is sent and the status of the LEDs will be displayed on the web page.
      setTimeout(updateStatus, 2000);
      
      //________________________________________________________________________________ btn_Click(id,cmd)
      function btn_Click(id,cmd) {
        Send("set","id=" + id + "&cmd=" + cmd);
        setTimeout(updateStatus, 2000);
      }
      //________________________________________________________________________________ 

      //________________________________________________________________________________ updateStatus()
      function updateStatus() {
        // Command to get the latest LED status that the Master has received from the Slaves.
        Send("get","id=ALL&cmd=GLLS");
      }
      //________________________________________________________________________________ 
      
      //________________________________________________________________________________ implementing_Response(responseText)
      function implementing_Response(responseText) {
        // Example of response received from server is : 0|1|0|0
        // 0-> 0 = status "LED 1" on ESP32 A or Slave A.
        // 1-> 1 = status "LED 2" on ESP32 A or Slave A.
        // 2-> 0 = status "LED 1" on ESP32 B or Slave B.
        // 3-> 0 = status "LED 2" on ESP32 B or Slave B.
        // 
        // So :
        // const LEDs_Sta = responseText.split("|");
        // LEDs_Sta[0] = status "LED 1" on ESP32 A or Slave A.
        // LEDs_Sta[1] = status "LED 2" on ESP32 A or Slave A.
        // LEDs_Sta[2] = status "LED 1" on ESP32 B or Slave B.
        // LEDs_Sta[3] = status "LED 2" on ESP32 B or Slave B.
        
        const LEDs_Sta = responseText.split("|");
        
        var LED_1_ESP32_A_Sta = document.getElementById("sta_LED_1_ESP32_A");
        var LED_2_ESP32_A_Sta = document.getElementById("sta_LED_2_ESP32_A");
        var LED_1_ESP32_B_Sta = document.getElementById("sta_LED_1_ESP32_B");
        var LED_2_ESP32_B_Sta = document.getElementById("sta_LED_2_ESP32_B");
        
        if (LEDs_Sta[0] == "1") {
          LED_1_ESP32_A_Sta.innerHTML = "ON";
          LED_1_ESP32_A_Sta.style.color = "#1976D2";
        } else if (LEDs_Sta[0] == "0") {
          LED_1_ESP32_A_Sta.innerHTML = "OFF";
          LED_1_ESP32_A_Sta.style.color = "#F5004F";
        }
        
        if (LEDs_Sta[1] == "1") {
          LED_2_ESP32_A_Sta.innerHTML = "ON";
          LED_2_ESP32_A_Sta.style.color = "#1976D2";
        } else if (LEDs_Sta[1] == "0") {
          LED_2_ESP32_A_Sta.innerHTML = "OFF";
          LED_2_ESP32_A_Sta.style.color = "#F5004F";
        }
        
        if (LEDs_Sta[2] == "1") {
          LED_1_ESP32_B_Sta.innerHTML = "ON";
          LED_1_ESP32_B_Sta.style.color = "#1976D2";
        } else if (LEDs_Sta[2] == "0") {
          LED_1_ESP32_B_Sta.innerHTML = "OFF";
          LED_1_ESP32_B_Sta.style.color = "#F5004F";
        }
        
        if (LEDs_Sta[3] == "1") {
          LED_2_ESP32_B_Sta.innerHTML = "ON";
          LED_2_ESP32_B_Sta.style.color = "#1976D2";
        } else if (LEDs_Sta[3] == "0") {
          LED_2_ESP32_B_Sta.innerHTML = "OFF";
          LED_2_ESP32_B_Sta.style.color = "#F5004F";
        }
      }
      //________________________________________________________________________________ 
      
      //________________________________________________________________________________ Send(sta, msg)
      function Send(sta, msg) {
        if (window.XMLHttpRequest) {
          // code for IE7+, Firefox, Chrome, Opera, Safari.
          xmlhttp = new XMLHttpRequest();
        } else {
          // code for IE6, IE5.
          xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
        }
        xmlhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            if (sta == "get") {
              implementing_Response(this.responseText);
            }
          }
        }
        xmlhttp.open("GET", "commands?" + msg, true);
        xmlhttp.send();
      }
      //________________________________________________________________________________
    </script>
  </body>
</html>
)=====";