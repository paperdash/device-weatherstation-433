<template>
  <v-app>
    <template v-if="isLoading">
      <v-overlay absolute>
        <v-progress-circular
          indeterminate
          size="64"
        />
      </v-overlay>
    </template>
    <template v-else>
      <component :is="layout" />
    </template>
  </v-app>
</template>

<script>
  import { mapState, mapActions } from 'vuex'
  import '@/assets/app.css'
  import notifications from '@/components/Notifications'

  export default {
    components: {
      // eslint-disable-next-line vue/no-unused-components
      'layout-default': () => import('@/layouts/default'),
      // eslint-disable-next-line vue/no-unused-components
      'layout-setup': () => import('@/layouts/setup'),
      notifications,
    },
    data: () => ({
      isLoading: true,
    }),
    computed: {
      ...mapState({
        stats: state => state.app.stats,
      }),
      layout () {
        return 'layout-' + (this.$route.meta.layout || 'default')
      },
    },
    created () {
      Promise.all([this.loadStats(), this.loadSettings()]).then(() => {
        // init app
        let goto
        const setupFinished = true // this.stats.device.configured TODO
        const wifiConnected = this.stats.wifi.connected

        // only on initial navigation
        if (!setupFinished) {
          if (this.$route.path !== '/setup/start') {
            goto = '/setup/start'
          }
        } else if (!wifiConnected) {
          if (this.$route.path !== '/wifi') {
            goto = '/wifi'
          }
        }

        if (goto) {
          this.$router.push(goto)
        }

        // initialization done
        this.isLoading = false
      })
    },
    methods: {
      ...mapActions({
        loadStats: 'app/loadStats',
        loadSettings: 'app/loadSettings',
      }),
    },
  }
</script>

<style scoped>
</style>
