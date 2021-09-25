import styled, { createGlobalStyle } from 'styled-components'
import DirTree from './components/DirTrees';

const GlobalStyle = createGlobalStyle`
  * {
    padding: 0;
    margin: 0;
  }
`

const AppWrapper = styled.div`
  display: flex;
  justify-content: space-around;
  height: 100vh;
  > div {
    width: 100%;
    padding: 10px 20px;
  }
`

function App() {
  return (
    <AppWrapper>
      <GlobalStyle />
      <DirTree title='源文件' bgColor='#B5A28A' />
      <DirTree title='备份文件' bgColor='#254043' />
    </AppWrapper>
  );
}

export default App;
