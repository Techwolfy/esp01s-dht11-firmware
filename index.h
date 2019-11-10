// HTML Index Page Resource
const char *HTML = R"EOF(
<html>
    <head>
        <title>ESP-01S Temperature Sensor</title>
        <link href='https://cdnjs.cloudflare.com/ajax/libs/bootswatch/4.3.1/darkly/bootstrap.min.css' rel='stylesheet'>
    </head>
    <script type='text/javascript'>
        function controlPin(pin, state) {
            var uri = '/control?pin=' + pin;
            if (state != undefined) {
                uri += '&state=' + state;
            }

            var xhr = new XMLHttpRequest();
            xhr.open('GET', uri);
            xhr.send();

            refreshStatus();
        }

        function refreshStatus() {
            var xhr = new XMLHttpRequest();
            xhr.open('GET', '/status');
            xhr.onreadystatechange = function() {
                if (xhr.readyState == 4) {
                    if (xhr.status == 200) {
                        displayStatus(JSON.parse(xhr.response));
                    } else {
                        displayStatus();
                    }
                }
            }
            xhr.send();
        }

        function displayStatus(status) {
            var statusBadge = document.getElementById('statusBadge');
            if (status == undefined) {
                statusBadge.className = 'btn btn-danger';
                statusBadge.innerText = 'Disconnected';
                return;
            } else {
                statusBadge.className = 'btn btn-success';
                statusBadge.innerText = 'Connected';
            }

            var statusCard = document.getElementById('statusCard');
            var greenHtml = '<span class=\'badge badge-success\'>';
            var redHtml = '<span class=\'badge badge-danger\'>';
            var endHtml = '</span>';
            var statusHtml = '';

            statusHtml += 'Temperature: ' + greenHtml + status.temperature.toFixed(2) + ' &deg;C' + endHtml + '&nbsp;';
            statusHtml += greenHtml + ((status.temperature * 9/5) + 32).toFixed(2) + ' &deg;F' + endHtml + '<br>\n';
            statusHtml += 'Humidity: ' + greenHtml + status.humidity.toFixed(2) + ' %RH' + endHtml + '<br>\n';
            statusHtml += 'LED state: ' + (status.ledActive ? greenHtml + 'ON' + endHtml : redHtml + 'OFF' + endHtml) + '<p>\n';

            statusHtml += 'Hostname: ' + status.hostName + '<br>\n';
            statusHtml += 'MAC address: ' + status.macAddress + '<br>\n';
            statusHtml += 'SSID: ' + status.networkSSID + '<br>\n';
            statusHtml += 'IP address: ' + status.ipAddress + '<br>\n';
            statusHtml += 'Subnet mask: ' + status.subnetMask + '<br>\n';
            statusHtml += 'Default gateway: ' + status.gatewayAddress + '<br>\n';

            statusCard.innerHTML = statusHtml;
        }

        setInterval(refreshStatus, 10000);
    </script>
    <body>
        <div class='navbar navbar-expand-lg navbar-dark bg-primary mb-3'>
            <div class='container'>
                <span class='navbar-brand'>ESP-01S Temperature Sensor</span>
                <div class='navbar-right'>
                    <span id='statusBadge' class='btn btn-danger' style='pointer-events: none;'>Disconnected</span>
                </div>
            </div>
        </div>
        <div class='container'>
            <div class='card mb-3'>
                <div class='card-body'>
                    <h4 class='card-title'>Status</h4>
                    <div id='statusCard' class='card-text'>Loading...</div>
                </div>
            </div>
            <div class='card mb-3'>
                <div class='card-body'>
                    <h4 class='card-title'>Control</h4>
                    <button class='btn btn-primary mr-3' onclick='controlPin(0);'>Toggle LED</button>
                </div>
            </div>
        </div>
    </body>
</html>
)EOF";
