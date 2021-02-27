<template>
  <v-container fluid>
    <v-snackbar
      v-model="isSnackbar"
      :timeout="3000"
      color="success"
    >
      i8n:saved
    </v-snackbar>

    <v-card
      class="mx-auto"
      width="400"
      flat
    >
      <v-card-text>
        TODO:: like fritzbox update view
      </v-card-text>

      <v-card-text>
        <v-dialog
          v-model="dialogSystemUpdate"
          max-width="400"
        >
          <template v-slot:activator="{ on }">
            <v-btn
              class="my-5"
              block
              small
              outlined
              color="warning"
              v-on="on"
            >
              System update
            </v-btn>
          </template>
          <v-card :loading="system.updateProgress > 0">
            <template v-slot:progress>
              <v-progress-linear
                v-model="system.updateProgress"
                :indeterminate="system.updateProgress === 100"
                height="10"
              />
            </template>
            <v-card-title class="headline">
              System update
            </v-card-title>
            <v-card-text>
              <!--
              <v-alert border="left" type="warning" outlined>
                Sed in libero ut nibh placerat accumsan. Phasellus leo dolor, tempus non, auctor et
              </v-alert>
              -->

              <v-file-input
                v-model="system.firmware"
                show-size
                accept="application/octet-stream"
                label="Firmware"
              />
            </v-card-text>
            <v-card-actions>
              <v-btn
                text
                href="https://github.com/paperdash"
                target="_blank"
              >
                Get firmware
              </v-btn>
              <v-spacer />
              <v-btn
                ref="firmware"
                outlined
                color="warning"
                @click="onSystemUpdate()"
              >
                Update system
              </v-btn>

              <!--
              <v-btn color="green darken-1" text @click="dialogSystemUpdate = false">Disagree</v-btn>
              <v-btn color="green darken-1" text @click="dialogSystemUpdate = false">Agree</v-btn>
              -->
            </v-card-actions>
          </v-card>

          <v-dialog
            :value="system.updateResult !== null"
            persistent
            max-width="400"
          >
            <v-card>
              <v-card-title class="headline">
                update result...
                {{ system.updateResult }}
              </v-card-title>

              <v-card-actions>
                <v-spacer />
                <v-btn
                  text
                  @click="onUpdateDone()"
                >
                  OK
                </v-btn>
              </v-card-actions>
            </v-card>
          </v-dialog>
        </v-dialog>
      </v-card-text>

      <v-card-actions>
        <v-btn
          text
          color="primary"
          class="_px-0 my-2"
          @click="onSave()"
        >
          i8n:Save
        </v-btn>

        <!--
        <v-spacer></v-spacer>

        <v-btn text color="primary" @click="onSave()">i8n:Save</v-btn>
        -->
      </v-card-actions>
    </v-card>
  </v-container>
</template>

<script>
  import apiDevice from '../api/device'
  import axios from 'axios'

  export default {
    name: 'Settings',
    components: {
    },
    data: () => ({
      isLoading: true,
      isSnackbar: false,
      tab: 0,

      settings: null,

      // 0 thru 3 corresponding to 4 cardinal rotations
      deviceOrientation: [
        { text: 'Nord', value: 0 },
        { text: 'East', value: 1 },
        { text: 'South', value: 2 },
        { text: 'West', value: 3 },
      ],
      deviceTheme: [
        { text: 'Black', value: 'black' },
        { text: 'White', value: 'white' },
      ],
      deviceMode: [
        { text: 'Active', value: 'active' },
        { text: 'Passive', value: 'passive' },
      ],

      // @see https://openweathermap.org/current#multi
      weatherLang: [
        { text: 'English', value: 'en' },
        { text: 'Deutsch', value: 'de' },
      ],
      weatherUnit: [
        { text: 'Imperial', value: '' },
        { text: 'Metrisch', value: 'metric' },
      ],

      dialogSystemUpdate: false,
      system: {
        firmware: null,
        updateProgress: 0,
        updateResult: null,
      },
    }),
    created () {
      apiDevice.getSettings(settings => {
        this.settings = settings

        this.isLoading = false
      })
    },
    methods: {
      onSave () {
        this.isLoading = true

        apiDevice.putSettings(this.settings, data => {
          console.log(data)
          this.isLoading = false
          this.isSnackbar = true

          this.$router.push('/')
        })
      },

      onSystemUpdate () {
        const self = this
        self.system.updateProgress = 0

        const config = {
          onUploadProgress: function (progressEvent) {
            const percentCompleted = Math.round(
              (progressEvent.loaded * 100) / progressEvent.total,
            )

            self.system.updateProgress = percentCompleted
          },
        }

        const formData = new FormData()
        formData.append('update', this.system.firmware)

        axios
          .post('/update', formData, config)
          .then(response => {
            console.log(response.data)

            self.system.updateProgress = null
            self.system.updateResult = response.data.success
          })
          .catch(response => {
            console.log(response)

            self.system.updateProgress = null
            self.system.updateResult = false
          })
      },

      onUpdateDone () {
        if (this.system.updateResult === true) {
          window.location = '/'
        } else {
          this.system.firmware = null
          this.system.updateProgress = 0
          this.system.updateResult = null
        }
      },
    },
  }
</script>

<style scoped>
>>> .v-input.text-right .v-text-field__slot > input {
  text-align: right;
}
</style>
