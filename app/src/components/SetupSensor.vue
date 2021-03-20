<template>
  <v-card :loading="loading">
    <v-toolbar
      color="orange"
      dark
      flat
    >
      <v-toolbar-title>Edit sensor</v-toolbar-title>

      <div class="flex-grow-1" />

      <v-btn
        icon
        @click="btnCancel"
      >
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
          <v-tab href="#config">
            Add
          </v-tab>
          <v-tab href="#replace">
            Replace
          </v-tab>
        </v-tabs>
      </template>
    </v-toolbar>

    <template v-if="sensorEdit">
      <v-tabs-items v-model="action">
        <v-tab-item value="config">
          <v-card-text>
            <v-text-field
              v-model="sensorEdit.label"
              label="i8n:Name"
            />
          </v-card-text>
        </v-tab-item>

        <v-tab-item value="replace">
          <v-card-text>
            <v-select
              v-model="replaceSensor"
              label="i8n:Available sensors"
              :items="sensors"
              return-object
              item-value="id"
              dense
            >
              <template v-slot:item="data">
                {{ data.item.label }}
                <v-spacer />
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
        <v-spacer />

        <v-btn
          color="black"
          text
          dense
          @click="btnCancel"
        >
          Cancel
        </v-btn>

        <v-btn
          color="blue accent-4"
          text
          dense
          @click="btnAccept"
        >
          Save
        </v-btn>
      </v-card-actions>
    </template>
  </v-card>
</template>

<script>
  import { mapActions } from 'vuex'

  export default {
    props: {
      sensor: {
        type: Object,
        required: true,
      },
      sensors: {
        type: Array,
        required: true,
      },

      onCommit: {
        type: Function,
        required: true,
      },
    },
    data: () => ({
      loading: false,
      sensorEdit: null,

      action: null,
      replaceSensor: null,
    }),
    watch: {
      sensor: function (sensor) {
        this.initEdit(sensor)
      },
    },
    mounted () {
      this.initEdit(this.sensor)
    },

    methods: {
      ...mapActions({
        updateSensor: 'sensors/update',
      }),

      initEdit: function (sensor) {
        // clone for editing
        this.sensorEdit = { ...sensor }

        this.loading = false
        this.action = 'action'
        this.replaceSensor = null
      },

      btnCancel: function () {
        this.$emit('done')
      },

      btnAccept: function () {
        this.loading = true

        if (this.action === 'replace') {
          // use settings from old existing
          this.sensorEdit.label = this.replaceSensor.label
        }

        // TODO refactor
        this.updateSensor([this.sensorEdit.id, this.sensorEdit]).then(() => {
          if (this.action === 'replace') {
            this.$store.commit('sensors/delete', this.replaceSensor.id)
          }

          this.$emit('done')
          this.loading = false

          this.$store.commit('notification', 'saved')
        })
      },
    },
  }
</script>

<style scoped>

</style>
