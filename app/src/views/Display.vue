<template>
  <v-container fluid>
    <v-snackbar v-model="isSnackbar" :timeout="3000" color="success">i8n:saved</v-snackbar>

    <v-row no-gutters justify="center">
      <v-col lg="5" md="6" sm="8">
        <v-card flat>
          <v-card-title class="display-2 mb-12 justify-center text-center">
            Choose a Display
          </v-card-title>

          <v-skeleton-loader
            v-if="isLoading"
            type="list-item-two-line,list-item-two-line,list-item-two-line"
            class="mx-auto"
          ></v-skeleton-loader>

          <v-list v-else>
            <template v-for="(display, i) in displayAvailable">
              <div :key="i">
                <v-divider v-if="i > 0"></v-divider>

                <v-list-item class="px-1" @click="onDisplayConnect(display.host)">
                  <!--
                  <v-list-item-icon class="mr-2 ml-2">
                    <v-icon v-if="display.host === settings.display.host">$check</v-icon>
                  </v-list-item-icon>
                  -->

                  <v-list-item-content dark>
                    <v-list-item-title v-text="display.host"></v-list-item-title>
                    <v-list-item-subtitle v-text="display.ip"></v-list-item-subtitle>
                  </v-list-item-content>

                  <v-list-item-icon>
                    <v-icon v-if="display.host === settings.display.host">$check</v-icon>

                    <v-icon
                      v-else-if="display.host !== connectingHost"
                      class="ml-3">$next</v-icon>

                    <v-progress-circular
                      v-else-if="display.host === connectingHost"
                      :size="24"
                      :width="2"
                      color="grey "
                      indeterminate
                    ></v-progress-circular>

                  </v-list-item-icon>
                </v-list-item>
              </div>
            </template>

            <v-divider></v-divider>

            <v-card-actions class="pa-0">
              <v-btn text color="primary" class="my-2">Choose Another Display</v-btn>
              <v-spacer></v-spacer>
              <v-btn text outlined class="my-2" @click="onDisplayTest()">Test</v-btn>
            </v-card-actions>
          </v-list>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script>
import apiDevice from "@/api/device";

export default {
  name: "Display",
  data: () => ({
    isLoading: true,
    isSnackbar: false,

    settings: null,
    connectingHost: "",

    displayAvailable: [],
  }),
  created() {
    apiDevice.getSettings(settings => {
      this.settings = settings;

      apiDevice.displayScan(list => {
        this.displayAvailable = list;

        this.isLoading = false;
      });
    });
  },
  methods: {
    onDisplayConnect(host) {
      this.connectingHost = host
      apiDevice.displayConnect(host, () => {
        this.settings.display.host = host
        this.connectingHost = ""
      })
    },
    onDisplayTest() {
      apiDevice.displayUpdate(() => {})
    }
  }
};
</script>

<style scoped>

</style>
