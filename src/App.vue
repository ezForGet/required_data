<template>
  <div style="padding: 40px; max-width: 800px; margin: 0 auto;">
    <h1>Система модальных окон</h1>
    
    <div style="display: flex; gap: 10px; margin-bottom: 30px; flex-wrap: wrap;">
      <AppButton @click="openSimple">Простое окно</AppButton>
      <AppButton @click="openScroll">Окно со скроллом</AppButton>
      <AppButton @click="openTall">Высокое окно</AppButton>
      <AppButton @click="openCascade(1)">Каскадные окна</AppButton>
    </div>

    <div v-if="modals.length > 0" style="margin-bottom: 20px;">
      Открыто окон: {{ modals.length }}
      <button @click="closeAll" style="margin-left: 10px; padding: 5px 10px;">
        Закрыть все
      </button>
    </div>

    <template v-for="(modal, index) in modals" :key="modal.id">
      <AppModal
        :visible="index === modals.length - 1"
        :title="modal.title"
        @close="close(modal.id)"
      >
        <template #body>
          <div v-if="modal.title === 'Простое окно'">
            <p>Это простое модальное окно.</p>
            <p>Его можно закрыть:</p>
            <ul>
              <li>Крестиком справа сверху</li>
              <li>Кликом вне окна</li>
              <li>Клавишей Esc</li>
            </ul>
          </div>

          <div v-else-if="modal.title === 'Окно со скроллом'" style="max-height: 300px; overflow-y: auto;">
            <div v-for="i in 15" :key="i" style="padding: 10px; margin-bottom: 10px; background: #f3f4f6; border-radius: 4px;">
              <strong>Раздел {{ i }}</strong>
              <p>Много текста для демонстрации скролла внутри модального окна.</p>
            </div>
          </div>

          <div v-else-if="modal.title === 'Высокое окно'">
            <div v-for="i in 10" :key="i" style="padding: 30px; margin-bottom: 20px; background: linear-gradient(135deg, #667eea, #764ba2); color: white; border-radius: 8px; text-align: center;">
              <h3 style="margin: 0;">Блок {{ i }}</h3>
            </div>
          </div>

          <div v-else-if="modal.title.startsWith('Окно №')">
            <p>Это {{ modal.title }}</p>
          </div>
        </template>

        <template v-if="modal.title.startsWith('Окно №')" #footer>
          <button @click="openCascade(getNumber(modal.title) + 1)" style="padding: 8px 16px; background: #3b82f6; color: white; border: none; border-radius: 4px; cursor: pointer;">
            Открыть ещё
          </button>
          <button @click="close(modal.id)" style="padding: 8px 16px; background: #6b7280; color: white; border: none; border-radius: 4px; cursor: pointer;">
            Закрыть это окно
          </button>
          <button @click="closeAll" style="padding: 8px 16px; background: #ef4444; color: white; border: none; border-radius: 4px; cursor: pointer;">
            Закрыть все окна
          </button>
        </template>
      </AppModal>
    </template>
  </div>
</template>

<script setup lang="ts">
import AppButton from './components/AppButton.vue'
import AppModal from './components/AppModal.vue'
import { useModalStore } from './components/modalStore'

const { modals, open, close, closeAll } = useModalStore()

const openSimple = () => open('Простое окно')
const openScroll = () => open('Окно со скроллом')
const openTall = () => open('Высокое окно')
const openCascade = (num: number) => open(`Окно №${num}`)

const getNumber = (title: string) => {
  const match = title.match(/№(\d+)/)
  return match ? parseInt(match[1]) : 0
}
</script>
