import { reactive } from 'vue'

interface ModalItem {
  id: number
  title: string
}

const state = reactive({
  modals: [] as ModalItem[],
  nextId: 1
})

export function useModalStore() {
  const open = (title: string) => {
    state.modals.push({ id: state.nextId++, title })
    return state.modals[state.modals.length - 1].id
  }

  const close = (id?: number) => {
    if (id) {
      const index = state.modals.findIndex(m => m.id === id)
      if (index !== -1) {
        state.modals.splice(index, 1)
      }
    } else if (state.modals.length > 0) {
      state.modals.pop()
    }
  }

  const closeAll = () => {
    state.modals.splice(0, state.modals.length)
  }

  return {
    modals: state.modals,
    currentModal: state.modals.length > 0 ? state.modals[state.modals.length - 1] : null,
    open,
    close,
    closeAll
  }
}