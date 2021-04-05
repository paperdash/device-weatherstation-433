<template>
  <v-card
    flat
    rounded="lg"
    class="pa-5"
  >
    <v-card-title class="text-h3 mb-12 justify-center text-center">
      Device settings
    </v-card-title>

    <div>
      <v-select
        v-model="form.theme"
        :items="optionsTheme"
        label="Appearance"
        prepend-icon="$palette"
        disabled
      />

      <v-select
        v-model="form.language"
        disabled
        :items="getAvailableLanguages"
        item-text="native"
        item-value="code"
        label="Language"
        prepend-icon="$translate"
      />

      <v-autocomplete
        v-model="form.timezone"
        :items="getAvailableTimezonesSorted"
        item-value="name"
        item-text="name"
        label="Timezone"
        prepend-icon="$access_time"
        return-object
      >
        <template #item="{ item }">
          (GMT{{ item.utcOffsetStr }}) {{ item.name }}
        </template>
        <template #selection="{item}">
          (GMT{{ item.utcOffsetStr }}) {{ item.name }}
        </template>
      </v-autocomplete>
    </div>

    <v-divider class="mt-12" />
    <v-card-actions>
      <v-btn
        text
        @click="resetChanges"
      >
        Restore
      </v-btn>
      <v-spacer />
      <v-btn
        :loading="isProcessing"
        depressed
        @click="commitChanges"
      >
        <v-icon left>
          $done
        </v-icon>
        Save
      </v-btn>
    </v-card-actions>
  </v-card>
</template>

<script>
  import { mapState, mapGetters, mapMutations, mapActions } from 'vuex'

  export default {
    data: () => ({
      isProcessing: false,
      form: {
        theme: '',
        country: '',
        timezone: '',
        language: '',
      },

      optionsTheme: [
        { text: 'Black', value: 'black' },
        { text: 'White', value: 'white' },
      ],
      deviceMode: [
        { text: 'Active', value: 'active' },
        { text: 'Passive', value: 'passive' },
      ],
    }),
    computed: {
      ...mapState({
        settings: state => state.app.settings,
      }),
      ...mapGetters({
        getAvailableCountries: 'app/getAvailableCountries',
        getAvailableLanguages: 'app/getAvailableLanguages',
        getAvailableTimezones: 'app/getAvailableTimezones',
        getTimezone: 'app/getTimezone',
      }),

      getAvailableTimezonesSorted () {
        return this.getAvailableTimezones.slice(0).sort((a, b) => {
          if (a.utcOffset === b.utcOffset) {
            return 0
          } else {
            return a.utcOffset < b.utcOffset ? -1 : 1
          }
        })
      },
    },
    created () {
      this.resetChanges()
    },
    methods: {
      ...mapMutations({
        updateSettings: 'app/updateSettings',
      }),
      ...mapActions({
        saveSettings: 'app/saveSettings',
      }),
      commitChanges () {
        this.isProcessing = true

        this.updateSettings({
          device: {
            name: this.form.name,
            theme: this.form.theme,
          },
          system: {
            country: this.form.country,
            language: this.form.language,
            timezone: this.form.timezone.name,
            utc: this.form.timezone.utcOffset * 60,
            dst: this.form.timezone.dstOffset * 60,
          },
        })

        this.saveSettings().then(() => {
          this.isProcessing = false
        })
      },
      resetChanges () {
        this.form.theme = this.settings.device.theme
        this.form.country = this.settings.system.country
        this.form.timezone = this.getTimezone(this.settings.system.timezone)
        this.form.language = this.settings.system.language
      },
    },
  }
</script>

<style scoped>
</style>
