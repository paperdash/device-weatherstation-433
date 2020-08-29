<template>
  <v-card :loading="loading">
    <v-toolbar
      color="orange"
      dark
      flat
    >
      <v-toolbar-title>i8n:Edit sensor</v-toolbar-title>

      <div class="flex-grow-1"></div>

      <v-btn icon @click="btnCancel">
        <v-icon>mdi-close</v-icon>
      </v-btn>

      <template v-slot:extension>
        <v-tabs
          v-model="action"
          centered
          grow
          slider-color="black"
          background-color="transparent"
        >
          <v-tab href="#config">i8n:config</v-tab>
          <v-tab href="#replace">i8n:replace</v-tab>
        </v-tabs>
      </template>
    </v-toolbar>


    <template v-if="sensorEdit">
      <v-tabs-items v-model="action">
        <v-tab-item value="config">
          <v-card-text>
            <v-text-field
              label="i8n:Name"
              v-model="sensorEdit.label"
            ></v-text-field>
          </v-card-text>
        </v-tab-item>

        <v-tab-item value="replace">
          <v-card-text>
            <v-select
              label="i8n:Available sensors"
              :items="sensors"
              v-model="replaceSensor"
              dense
            >
              <template v-slot:item="data">
                {{ data.item.label }}
                <v-spacer></v-spacer>
                <template v-if="data.item.lastUpdate === 0">
                  <small class="red--text font-italic">i8n:offline</small>
                </template>
                <template v-else>
                  {{ data.item.temperature }}&deg;C / {{ data.item.humidity }} &percnt;
                </template>
              </template>
              <template v-slot:selection="data">
                {{ data.item.label }}
              </template>
            </v-select>

          </v-card-text>
        </v-tab-item>
      </v-tabs-items>

      <v-card-actions>
        <v-spacer></v-spacer>

        <v-btn
          color="black"
          text
          @click="btnCancel"
          dense
        >
          Abbrechen
        </v-btn>

        <v-btn
          color="blue accent-4"
          text
          @click="btnAccept"
          dense
        >
          Speichern
        </v-btn>
      </v-card-actions>
    </template>
  </v-card>

</template>

<script>
export default {
  props: {
    sensor: {
      type: Object,
      required: true
    },
    sensors: {
      type: Array,
      required: true
    },

    onCommit: {
      type: Function,
      required: true
    }
  },
  data: () => ({
    loading: false,
    sensorEdit: null,

    action: null,
    replaceSensor: null
  }),
  mounted() {
    this.initEdit(this.sensor)
  },
  watch: {
    sensor: function (sensor) {
      this.initEdit(sensor)
    }
  },

  methods: {
    initEdit: function(sensor) {
      // clone for editing
      this.sensorEdit = {...sensor}

      this.action = 'action'
      this.replaceSensor = null
    },

    btnCancel: function() {
      this.sensorEdit = null;
      this.$emit('done')
    },

    btnAccept: function () {
      this.loading = true
      //alert("accept");

      this.$store.dispatch('updateSensor', [this.sensorEdit.id, this.sensorEdit])

      // TODO snackbar...

/*
      let updateSensor
      switch (this.action) {
        case 'replace': {
          updateSensor = this.replaceSensor

          // device data
          updateSensor.deviceId = this.sensor.deviceId
          updateSensor.discoverId = this.sensor.discoverId

          // live data
          updateSensor.timestamp = this.sensor.timestamp
          updateSensor.temperature = this.sensor.temperature
          updateSensor.humidity = this.sensor.humidity

          // remove old entry
          let index = this.sensors.findIndex(sensor => sensor.id === this.sensor.id)
          Vue.delete(this.sensors, index);

          break;
        }

        case 'config': {
          updateSensor = this.sensor

          updateSensor.title = this.sensorEdit.title
          updateSensor.kind = this.sensorEdit.kind

          break;
        }
      }


      // TODO persist data
      if (updateSensor) {
        if (updateSensor.id) {
          // update
          api
            .putSensor(updateSensor.id, updateSensor)
            .then(() => {
              this.loading = false

              this.$emit('done')
            })
        } else {
          // add
          api
            .postSensor(updateSensor)
            .then((res) => {
              updateSensor.id = res.data.id
              this.loading = false

              this.$emit('done')
            })
        }
      }
 */

    }
  }
}
</script>

<style scoped>

</style>