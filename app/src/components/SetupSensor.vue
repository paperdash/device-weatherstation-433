<template>
  <v-card :loading="loading">
    <v-toolbar
      color="orange"
      dark
      flat
    >
      <v-toolbar-title>Edit sensor</v-toolbar-title>

      <div class="flex-grow-1"></div>

      <v-btn icon @click="btnCancel">
        <v-icon>$close</v-icon>
      </v-btn>

      <template v-slot:extension>
        <v-tabs
          v-model="action"
          centered
          grow
          slider-color="black"
          background-color="transparent"
        >
          <v-tab href="#config">Add</v-tab>
          <v-tab href="#replace">Replace</v-tab>
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
              return-object
              item-value="id"
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
          Cancel
        </v-btn>

        <v-btn
          color="blue accent-4"
          text
          @click="btnAccept"
          dense
        >
          Save
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

      this.loading = false
      this.action = 'action'
      this.replaceSensor = null
    },

    btnCancel: function() {
      this.$emit('done')
    },

    btnAccept: function () {
      this.loading = true

      if (this.action === 'replace') {
        // use settings from old existing
        this.sensorEdit.label = this.replaceSensor.label
      }

      this.$store.dispatch('putSensor', [this.sensorEdit.id, this.sensorEdit]).then(() => {

        if (this.action === 'replace') {
          this.$store.commit('deleteSensor', this.replaceSensor.id)
        }

        this.$emit('done')
        this.loading = false

        this.$store.commit('notification', "saved")
      })
    }
  }
}
</script>

<style scoped>

</style>