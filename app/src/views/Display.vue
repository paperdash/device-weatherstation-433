<template>
  <v-container fluid>
    <v-snackbar
      v-model="isSnackbar"
      :timeout="3000"
      color="success"
    >
      i8n:saved
    </v-snackbar>

    <v-row
      no-gutters
      justify="center"
    >
      <v-col
        lg="5"
        md="6"
        sm="8"
      >
        <v-card flat>
          <v-card-title class="display-2 mb-12 justify-center text-center">
            Choose a Display
          </v-card-title>

          <v-skeleton-loader
            v-if="isLoading"
            type="list-item-two-line,list-item-two-line,list-item-two-line"
            class="mx-auto"
          />

          <v-list v-else>
            <template v-for="(display, i) in displayAvailable">
              <div :key="i">
                <v-divider v-if="i > 0" />

                <v-list-item
                  class="px-1"
                  @click="onDisplayConnect(display.host)"
                >
                  <!--
                  <v-list-item-icon class="mr-2 ml-2">
                    <v-icon v-if="display.host === settings.display.host">$check</v-icon>
                  </v-list-item-icon>
                  -->

                  <v-list-item-content dark>
                    <v-list-item-title v-text="display.host" />
                    <v-list-item-subtitle v-text="display.ip" />
                  </v-list-item-content>

                  <v-list-item-icon>
                    <v-icon v-if="display.host === settings.display.host">
                      $check
                    </v-icon>

                    <v-icon
                      v-else-if="display.host !== connectingHost"
                      class="ml-3"
                    >
                      $next
                    </v-icon>

                    <v-progress-circular
                      v-else-if="display.host === connectingHost"
                      :size="24"
                      :width="2"
                      color="grey "
                      indeterminate
                    />
                  </v-list-item-icon>
                </v-list-item>
              </div>
            </template>

            <v-divider />

            <v-card-actions class="pa-0">
              <v-btn
                text
                color="primary"
                class="my-2"
              >
                Choose Another Display
              </v-btn>
              <v-spacer />
              <v-btn
                text
                outlined
                class="my-2"
                :disabled="isTesting"
                @click="onDisplayTest()"
              >
                <span v-show="!isTesting">Test</span>

                <v-progress-circular
                  v-show="isTesting"
                  :value="updateProgress"
                />
              </v-btn>
            </v-card-actions>
          </v-list>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script>
  import apiDevice from '@/api/device'

  export default {
    name: 'Display',
    data: () => ({
      isLoading: true,
      isSnackbar: false,
      isTesting: false,
      updateProgress: 0,

      settings: null,
      connectingHost: '',

      displayAvailable: [],
    }),
    created () {
      apiDevice.getSettings(settings => {
        this.settings = settings

        apiDevice.displayScan(list => {
          this.displayAvailable = list

          this.isLoading = false
        })
      })
    },
    methods: {
      onDisplayConnect (host) {
        this.connectingHost = host
        apiDevice.displayConnect(host, () => {
          this.settings.display.host = host
          this.connectingHost = ''
        })
      },
      onDisplayTest () {
        this.isTesting = true
        this.updateProgress = 12.5

        apiDevice.displayUpdate(() => {
          const timer = window.setInterval(() => {
            this.updateProgress += 12.5
          }, 1000)

          window.setTimeout(() => {
            this.isTesting = false

            timer.clear()
          }, 8000)
        })
      },
    },
  }
</script>

<style scoped>

</style>
