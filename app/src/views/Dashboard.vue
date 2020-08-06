<template>
  <v-container fluid grid-list-md pa-2>
    <v-layout wrap>
      <v-flex v-for="sensor in sensors" :key="sensor.id" xs12 sm6 md4 lg3>
        <v-card class="text-center my-14 mx-4">
          <v-card-text>
            <v-responsive
              class="mx-auto rounded-circle elevation-10 white"
              style="position: absolute; top: -60px; left: calc(50% - 50px);"
              height="100"
              width="100"
            >
              <p class="text-no-wrap text-h4 black--text mt-6 mb-0">{{ sensor.temperature }}°</p>
              <v-divider></v-divider>
              <p class="text-no-wrap text-subtitle-1 grey lighten-5 pb-5">{{ sensor.humidity }}%</p>
            </v-responsive>
          </v-card-text>

          <v-card-text class="text-h4 font-italic">Name</v-card-text>

          <v-divider></v-divider>

          <v-card-actions class="grey lighten-5 pa-3">
            <small
              :title="sensor.last_update | moment('LLLL')"
            >{{ sensor.last_update | moment("from") }}</small>
          </v-card-actions>
        </v-card>
      </v-flex>
    </v-layout>
  </v-container>
</template>

<script>
import apiDevice from "@/api/device";

export default {
  data: () => ({
    sensors: [],
    connection: null,
  }),
  created() {
    apiDevice.getSensors((list) => {
      this.sensors = list;
    });
/*
    console.log("Starting connection to WebSocket Server");
    this.connection = new WebSocket("ws://" + window.location.host + "/ws");

    this.connection.onmessage = function (event) {
      // TODO read sensor json data
      console.log(event.data);
    };
*/
    /*
    this.connection.onopen = function(event) {
      //console.log(event)
      //console.log("Successfully connected to the echo websocket server...")
		}
		*/
  },
};
</script>

<style scoped>
</style>