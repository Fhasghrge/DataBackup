import { useState } from 'react'
import styled from 'styled-components'

const mockTree = [
  {
    filename: 'README.md',
  }, {
    filename: 'examples'
  },{
    filename: 'package.json'
  },{
    filename: 'test'
  },{
    filename: 'azure-pipelines.yml'
  },{
    filename: 'jest.config.js'
  },{
    filename: 'packages'
  },{
    filename: 'tsconfig.json'
  }
]

const TreeSelectWrapper = styled.div`
  padding: 20px 0;
`

const CurrentPathWrapper = styled.div`
  border: 1px solid red;
`

const ItemDir = styled.span`
  background-color: rgba(55,55,55, .4);
  display: inline-block;
`

const SubTreeWrapper = styled.ul`
  list-style: none;
  > li {
    margin-top: 10px;
    
  }
`

export default function TreeSelect({
  initPath = '/Users/shuang/Desktop/code'
}) {
  const [currentPath, setCurrentPath] = useState(initPath)
  const [currentTrees, setCurrentTrees] = useState(mockTree)
  return (
    <TreeSelectWrapper>
      <CurrentPathWrapper>
        <ItemDir> 🔙 </ItemDir>
        {currentPath.split('/').map((dir) => <ItemDir>{dir}</ItemDir>)}
      </CurrentPathWrapper>
      <SubTreeWrapper>
        {
          currentTrees.map(({filename}) => <li> {filename} </li>)
        }
      </SubTreeWrapper>
    </TreeSelectWrapper>
  )
}

