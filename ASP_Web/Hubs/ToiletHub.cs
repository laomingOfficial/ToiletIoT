using Microsoft.AspNetCore.SignalR;
using MQTTnet.Client;
using System.Threading.Tasks;
using ToiletIoT.Services;

namespace ToiletIoT.Hubs
{
    public class ToiletHub : Hub
    {
        private readonly IMqttClient _mqttClient;
        public ToiletHub(MqttClientService provider)
        {
            _mqttClient = provider.mqttClient;
        }

        public async Task SendCommand(string payload)
        {
            await _mqttClient.PublishAsync("api/request", payload);
        }
    }
}